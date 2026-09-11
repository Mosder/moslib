#define MOS_FORCE_PREFIXES
#include "moslib/ds/hashmap.h"

#include <string.h>

#include "moslib/safe.h"
#include "moslib/string.h"

typedef struct {
    size_t size;
    size_t cap;
    uint32_t (*hash)(const void *key);
    int (*eq)(const void *key1, const void *key2);
} Header;

Header *hm2hdr(const void *hm) {
    return (Header *)hm - 1;
}

void *hdr2hm(Header *hdr) {
    return hdr + 1;
}

uint32_t default_hash(const void *key, size_t key_size) {
    return 0;
}

int default_eq(const void *key1, const void *key2, size_t key_size) {
    return memcmp(key1, key2, key_size) == 0;
}

uint32_t string_hash(const void *key) {
    return 0;
}

int string_eq(const void *key1, const void *key2) {
    return strcmp(key1, key2) == 0;
}

uint32_t slice_hash(const void *key) {
    return 0;
}

int slice_eq(const void *key1, const void *key2) {
    return mos_ss_eq(*(MosStringSlice *)key1, *(MosStringSlice *)key2);
}

void *mos_hm_new_fn(MosHmInitArgs args) {
    Header *hdr = mos_safe_malloc(sizeof(Header));
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

    return hdr;
}

#define mos_hm_put(hm, key, ...)
#define mos_hm_put_e(hm, ...)
#define mos_hm_get(hm, key) 0
#define mos_hm_get_e(hm, key) NULL
#define mos_hm_first(hm) NULL
#define mos_hm_next(hm, curr) NULL

void mos_hm_free(void *hm) {
    (void)hm;
}
