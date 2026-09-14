#define MOS_FORCE_PREFIXES
#include "moslib/ds/hashmap.h"

#include <stdlib.h>
#include <string.h>

#include "moslib/safe.h"
#include "moslib/string.h"

#define INIT_CAP 16

typedef struct {
    uint8_t *filled;
    uint8_t *dead;
    size_t size;
    size_t cap;
    uint32_t (*hash)(const void *key);
    int (*eq)(const void *key1, const void *key2);
} Header;

static Header *hm2hdr(const void *hm) {
    return (Header *)hm - 1;
}

static void *hdr2hm(Header *hdr) {
    return hdr + 1;
}

static int filled(const void *hm, size_t index) {
    return hm2hdr(hm)->filled[index / 8] & (1 << index % 8);
}

static void fill(const void *hm, size_t index) {
    hm2hdr(hm)->filled[index / 8] |= (1 << index % 8);
}

static int dead(const void *hm, size_t index) {
    return hm2hdr(hm)->dead[index / 8] & (1 << index % 8);
}

static void kill(const void *hm, size_t index) {
    hm2hdr(hm)->dead[index / 8] |= (1 << index % 8);
}

static void unkill(const void *hm, size_t index) {
    hm2hdr(hm)->dead[index / 8] &= (0xFF ^ (1 << index % 8));
}

// FNV-1a
static uint32_t default_hash(const void *key, size_t key_size) {
    const uint8_t *p = key;
    uint32_t h = 2166136261;
    for (size_t i = 0; i < key_size; i++) {
        h ^= p[i];
        h *= 16777619;
    }
    return h;
}

static int default_eq(const void *key1, const void *key2, size_t key_size) {
    return memcmp(key1, key2, key_size) == 0;
}

static uint32_t string_hash(const void *key) {
    const char *str = *(char **)key;
    return default_hash(str, strlen(str));
}

static int string_eq(const void *key1, const void *key2) {
    return strcmp(*(char **)key1, *(char **)key2) == 0;
}

static uint32_t slice_hash(const void *key) {
    const MosStringSlice *slice = key;
    return default_hash(slice->data, slice->len);
}

static int slice_eq(const void *key1, const void *key2) {
    return mos_ss_eq(*(MosStringSlice *)key1, *(MosStringSlice *)key2);
}

void mos_hm_init(void *p_hm, size_t entry_size) {
    if (!*(void **)p_hm) {
        Header *hdr = mos_safe_malloc(sizeof(Header) + (INIT_CAP + 1) * entry_size);
        *hdr = (Header){
            .filled = mos_safe_calloc((INIT_CAP - 1) / 8 + 1, 1),
            .dead = mos_safe_calloc((INIT_CAP - 1) / 8 + 1, 1),
            .size = 0,
            .cap = INIT_CAP,
            .hash = NULL,
            .eq = NULL,
        };
        *(void **)p_hm = hdr2hm(hdr);
        return;
    }

    Header *hdr = hm2hdr(*(void **)p_hm);
    if (hdr->cap == 0) {
        hdr = mos_safe_realloc(hdr, sizeof(Header) + (INIT_CAP + 1) * entry_size);
        hdr->filled = mos_safe_calloc((INIT_CAP - 1) / 8 + 1, 1);
        hdr->dead = mos_safe_calloc((INIT_CAP - 1) / 8 + 1, 1);
        hdr->cap = INIT_CAP;
        *(void **)p_hm = hdr2hm(hdr);
    }
}

void *mos_hm_new_fn(MosHmInitArgs args) {
    Header *hdr = mos_safe_malloc(sizeof(Header));
    hdr->filled = NULL;
    hdr->dead = NULL;
    hdr->cap = 0;
    hdr->size = 0;

    switch (args.key) {
        case DEFAULT:
            hdr->hash = NULL;
            hdr->eq = NULL;
            break;
        case STR:
            hdr->hash = string_hash;
            hdr->eq = string_eq;
            break;
        case SS:
            hdr->hash = slice_hash;
            hdr->eq = slice_eq;
            break;
    }

    if (args.eq != NULL)
        hdr->eq = args.eq;
    if (args.hash != NULL)
        hdr->hash = args.hash;

    return hdr2hm(hdr);
}

static size_t probe(const void *hm, const void *key, size_t entry_size, size_t key_size, size_t key_off, int put) {
    Header *hdr = hm2hdr(hm);
    uint32_t hash = hdr->hash ? hdr->hash(key) : default_hash(key, key_size);
    size_t i = hash % hdr->cap;

    size_t tomb = -1;
    for (size_t it = 0; it < hdr->cap; it++) {
        void *key2 = (char *)hm + (i + 1) * entry_size + key_off;
        if (!filled(hm, i))
            return tomb != (size_t)-1 ? tomb : i;
        if (hdr->eq ? hdr->eq(key, key2) : default_eq(key, key2, key_size))
            return i;
        if (put && tomb == (size_t)-1 && dead(hm, i))
            tomb = i;
        i = (i + 1) % hdr->cap;
    }

    // NOTE: unreachable - rehash always happens after 100% fill, even at 100% load factor
    return -1;
}

static void rehash(void *p_hm, size_t entry_size, size_t key_size, size_t key_off, uint8_t load_factor) {
    void *hm = *(void **)p_hm;
    Header *hdr = hm2hdr(hm);
    size_t size = mos_hm_size(hm);
    size_t new_cap = hdr->cap;
    if (size * 400 >= new_cap * 3 * load_factor) {
        new_cap *= 2;
    }
    else {
        while (new_cap > INIT_CAP && size * 400 <= new_cap * load_factor)
            new_cap /= 2;
    }

    Header *new_hdr = mos_safe_malloc(sizeof(Header) + (new_cap + 1) * entry_size);
    *new_hdr = (Header){
        .filled = mos_safe_calloc((new_cap - 1) / 8 + 1, 1),
        .dead = mos_safe_calloc((new_cap - 1) / 8 + 1, 1),
        .size = size,
        .cap = new_cap,
        .hash = hdr->hash,
        .eq = hdr->eq,
    };
    void *new_hm = hdr2hm(new_hdr);

    for (size_t i = 0; i < hdr->cap; i++) {
        if (!filled(hm, i) || dead(hm, i))
            continue;

        void *entry = (char *)hm + (i + 1) * entry_size;
        void *key = (char *)entry + key_off;

        uint32_t hash = new_hdr->hash ? new_hdr->hash(key) : default_hash(key, key_size);
        size_t j = hash % new_hdr->cap;
        for (size_t it = 0; it < new_hdr->cap; it++) {
            if (!filled(new_hm, j)) {
                memcpy((char *)new_hm + (j + 1) * entry_size, entry, entry_size);
                fill(new_hm, j);
                break;
            }
            j = (j + 1) % new_hdr->cap;
        }
    }

    free(hdr->filled);
    free(hdr->dead);
    free(hdr);
    *(void **)p_hm = new_hm;
}

void mos_hm_put_fn(void *p_hm, void *key, size_t entry_size, size_t key_size, uint8_t load_factor) {
    size_t key_off = (char *)key - *(char **)p_hm;
    Header *hdr = hm2hdr(*(void **)p_hm);

    void *hm = *(void **)p_hm;
    size_t i = probe(hm, key, entry_size, key_size, key_off, 1);
    memcpy((char *)hm + (i + 1) * entry_size, hm, entry_size);
    if (!filled(hm, i)) {
        hdr->size++;
        fill(hm, i);
    }
    unkill(hm, i);

    if (hdr->size * 100 >= hdr->cap * load_factor)
        rehash(p_hm, entry_size, key_size, key_off, load_factor);
}

size_t mos_hm_get_fn(void *hm, void *key, size_t entry_size, size_t key_size) {
    size_t key_off = (char *)key - (char *)hm;
    size_t i = probe(hm, key, entry_size, key_size, key_off, 0);
    if (!filled(hm, i) || dead(hm, i)) {
        memset(hm, 0, entry_size);
        return 0;
    }
    return i + 1;
}

void *mos_hm_get_e_fn(void *hm, void *key, size_t entry_size, size_t key_size) {
    size_t key_off = (char *)key - (char *)hm;
    size_t i = probe(hm, key, entry_size, key_size, key_off, 0);
    if (!filled(hm, i) || dead(hm, i))
        return NULL;
    return (char *)hm + (i + 1) * entry_size;
}

int mos_hm_del_fn(void *hm, void *key, size_t entry_size, size_t key_size) {
    size_t key_off = (char *)key - (char *)hm;
    size_t i = probe(hm, key, entry_size, key_size, key_off, 0);
    if (!filled(hm, i) || dead(hm, i))
        return 0;
    kill(hm, i);
    return 1;
}

void *mos_hm_first_fn(const void *hm, size_t entry_size) {
    if (!hm)
        return NULL;

    Header *hdr = hm2hdr(hm);
    if (hdr->size == 0)
        return NULL;

    for (size_t i = 0; i < hdr->cap; i++) {
        if (filled(hm, i) && !dead(hm, i))
            return (char *)hm + (i + 1) * entry_size;
    }
    return NULL;
}

void *mos_hm_next_fn(const void *hm, const void *curr, size_t entry_size) {
    if (!hm)
        return NULL;

    Header *hdr = hm2hdr(hm);
    if (hdr->size == 0)
        return NULL;

    if (!curr)
        return mos_hm_first_fn(hm, entry_size);

    size_t index = ((char *)curr - (char *)hm) / entry_size;
    if (index >= hdr->cap)
        return NULL;

    for (size_t i = index; i < hdr->cap; i++) {
        if (filled(hm, i) && !dead(hm, i))
            return (char *)hm + (i + 1) * entry_size;
    }
    return NULL;
}

static size_t bit_count(const uint8_t *bitmap, size_t size) {
    size_t count = 0;
    for (size_t i = 0; i < size; i++) {
        uint8_t c = (bitmap[i] & 0x55) + ((bitmap[i] >> 1) & 0x55);
        c = (c & 0x33) + ((c >> 2) & 0x33);
        count += (c & 0x0F) + ((c >> 4) & 0x0F);
    }
    return count;
}

size_t mos_hm_size(const void *hm) {
    if (!hm)
        return 0;
    Header *hdr = hm2hdr(hm);
    if (hdr->cap == 0)
        return 0;
    return hdr->size - bit_count(hdr->dead, (hdr->cap - 1) / 8 + 1);
}

void mos_hm_free(void *hm) {
    if (!hm)
        return;

    Header *hdr = hm2hdr(hm);
    free(hdr->filled);
    free(hdr->dead);
    free(hdr);
}
