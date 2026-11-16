#define MOS_FORCE_PREFIXES
#include "moslib/ds/heap.h"

#include <stdlib.h>
#include <string.h>

#include "moslib/safe.h"

#define INIT_CAP 15

typedef struct {
    int min;
    size_t size;
    size_t cap;
    int (*cmp)(const void *el1, const void *el2);
} Header;

static Header *heap2hdr(const void *heap) {
    return (Header *)heap - 1;
}

static void *hdr2heap(Header *hdr) {
    return hdr + 1;
}

static int default_cmp(const void *el1, const void *el2, size_t key_size) {
    return memcmp(el1, el2, key_size);
}

// 1 if el1 should be el2's parent, 0 otherwise
#define comp(heap, i1, i2) comp_(heap, el_size, i1, i2)
static int comp_(void *heap, size_t el_size, size_t i1, size_t i2) {
    Header *hdr = heap2hdr(heap);

    void *el1 = (char *)heap + (i1 + 1) * el_size;
    void *el2 = (char *)heap + (i2 + 1) * el_size;
    int res = hdr->cmp ? hdr->cmp(el1, el2) : default_cmp(el1, el2, el_size);

    return (hdr->min && res < 0) || (!hdr->min && res > 0);
}

static size_t parent(size_t i) {
    return (i - 1) / 2;
}

static size_t left(size_t i) {
    return 2 * i + 1;
}

static size_t right(size_t i) {
    return 2 * i + 2;
}

#define swap(heap, i1, i2) swap_(heap, el_size, i1, i2)
static void swap_(void *heap, size_t el_size, size_t i1, size_t i2) {
    memcpy(heap, (char *)heap + (i1 + 1) * el_size, el_size);
    memcpy((char *)heap + (i1 + 1) * el_size, (char *)heap + (i2 + 1) * el_size, el_size);
    memcpy((char *)heap + (i2 + 1) * el_size, heap, el_size);
}

void mos_heap_init(void *p_heap, size_t el_size) {
    if (!*(void **)p_heap) {
        Header *hdr = mos_safe_malloc(sizeof(Header) + (INIT_CAP + 1) * el_size);
        *hdr = (Header){
            .min = 1,
            .size = 0,
            .cap = INIT_CAP,
            .cmp = NULL,
        };
        *(void **)p_heap = hdr2heap(hdr);
        return;
    }

    Header *hdr = heap2hdr(*(void **)p_heap);
    if (hdr->cap == 0) {
        hdr = mos_safe_realloc(hdr, sizeof(Header) + (INIT_CAP + 1) * el_size);
        hdr->cap = INIT_CAP;
        *(void **)p_heap = hdr2heap(hdr);
    }
}

void *mos_heap_new_fn(MosHeapNewArgs args) {
    Header *hdr = mos_safe_malloc(sizeof(Header));
    hdr->min = args.type == MOS_HEAP_MIN ? 1 : 0;
    hdr->size = 0;
    hdr->cap = 0;
    hdr->cmp = NULL;

    if (args.cmp != NULL)
        hdr->cmp = args.cmp;

    return hdr2heap(hdr);
}

static void *expand(void *heap, size_t el_size) {
    Header *hdr = heap2hdr(heap);
    hdr->cap = 2 * hdr->cap + 1;
    hdr = mos_safe_realloc(hdr, sizeof(Header) + (hdr->cap + 1) * el_size);
    return hdr2heap(hdr);
}

void mos_heap_add_fn(void *p_heap, size_t el_size) {
    void *heap = *(void **)p_heap;
    Header *hdr = heap2hdr(heap);
    if (hdr->size >= hdr->cap) {
        *(void **)p_heap = expand(heap, el_size);
        heap = *(void **)p_heap;
        hdr = heap2hdr(heap);
    }

    size_t i = hdr->size++;
    memcpy((char *)heap + (i + 1) * el_size, heap, el_size);

    while (i > 0 && comp(heap, i, parent(i))) {
        swap(heap, i, parent(i));
        i = parent(i);
    }
}

void mos_heap_pop_fn(void *heap, size_t el_size) {
    if (mos_heap_size(heap) == 0) {
        memset(heap, 0, el_size);
        return;
    }

    char temp[el_size];
    memcpy(temp, (char *)heap + el_size, el_size);
    mos_heap_del_top_fn(heap, el_size);
    memcpy(heap, temp, el_size);
}

int mos_heap_del_top_fn(void *heap, size_t el_size) {
    if (mos_heap_size(heap) == 0)
        return 0;

    Header *hdr = heap2hdr(heap);
    memcpy((char *)heap + el_size, (char *)heap + hdr->size-- * el_size, el_size);

    size_t i = 0;
    int changed;
    do {
        size_t l = left(i);
        size_t r = right(i);
        size_t new_par = i;

        if (l < hdr->size && comp(heap, l, new_par))
            new_par = l;
        if (r < hdr->size && comp(heap, r, new_par))
            new_par = r;

        if ((changed = new_par != i)) {
            swap(heap, new_par, i);
            i = new_par;
        }
    } while (changed);

    return 1;
}

size_t mos_heap_size(const void *heap) {
    if (!heap)
        return 0;

    Header *hdr = heap2hdr(heap);
    return hdr->size;
}

void mos_heap_free(void *heap) {
    if (!heap)
        return;

    Header *hdr = heap2hdr(heap);
    free(hdr);
}
