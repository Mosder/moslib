#define MOS_FORCE_PREFIXES
#include "moslib/ds/array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "moslib/safe.h"

MosArrHeader *arr2hdr(void *arr) {
    return (MosArrHeader *)arr - 1;
}

void *hdr2arr(MosArrHeader *hdr) {
    return hdr + 1;
}

void *init(size_t cap, size_t el_size) {
    MosArrHeader *hdr = mos_safe_malloc(sizeof(MosArrHeader) + cap * el_size);
    *hdr = (MosArrHeader){.len = 0, .cap = cap};
    return hdr2arr(hdr);
}

void *expand(void *arr, size_t min_cap, size_t el_size) {
    MosArrHeader *hdr = arr2hdr(arr);
    if (hdr->cap >= min_cap)
        return arr;

    if (hdr->cap < 1)
        hdr->cap = 1;

    while (hdr->cap < min_cap)
        hdr->cap *= 2;

    hdr = mos_safe_realloc(hdr, sizeof(MosArrHeader) + hdr->cap * el_size);
    return hdr2arr(hdr);
}

// clang-format off
// init array if it's not, define arr, hdr vars and expand to fit min_cap
#define ini(min_cap)                                                    \
    !*(void**)p_arr ? *(void **)p_arr = init(init_cap, el_size) : 0;    \
    MosArrHeader *hdr = arr2hdr(*(void**)p_arr);                        \
    *(void **)p_arr = expand(*(void**)p_arr, min_cap, el_size);         \
    void *arr = *(void **)p_arr;                                        \
    hdr = arr2hdr(arr)
// clang-format on

void mos_arr_append_fn(void *p_arr, size_t el_size, size_t init_cap) {
    ini(hdr->len + 1);
    hdr->len++;
    (void)arr;
}

void mos_arr_insert_fn(void *p_arr, size_t i, size_t el_size, size_t init_cap) {
    ini(hdr->len + 1);
    memmove((char *)arr + (i + 1) * el_size, (char *)arr + i * el_size, (hdr->len++ - i) * el_size);
}

void mos_arr_put_fn(void *p_arr, size_t i, size_t el_size, size_t init_cap) {
    ini(i + 1);
    if (i > hdr->len) {
        memset((char *)arr + hdr->len * el_size, 0, (i - hdr->len) * el_size);
        hdr->len = i + 1;
    }
}

size_t mos_arr_append_n_fn(void *p_arr, void *items, size_t n, size_t el_size, size_t init_cap) {
    ini(hdr->len + n);
    size_t i = hdr->len;

    memmove((char *)arr + i * el_size, items, n * el_size);
    hdr->len += n;
    return i;
}

size_t mos_arr_insert_n_fn(void *p_arr, size_t i, void *items, size_t n, size_t el_size, size_t init_cap) {
    ini(hdr->len + n);
    if (i >= hdr->len)
        return mos_arr_append_n_fn(p_arr, items, n, el_size, init_cap);

    memmove((char *)arr + (i + n) * el_size, (char *)arr + i * el_size, (hdr->len - i) * el_size);
    memmove((char *)arr + i * el_size, items, n * el_size);
    hdr->len += n;
    return i;
}

size_t mos_arr_put_n_fn(void *p_arr, size_t i, void *items, size_t n, size_t el_size, size_t init_cap) {
    ini(i + n + 1);
    if (i > hdr->len) {
        memset((char *)arr + hdr->len * el_size, 0, (i - hdr->len) * el_size);
    }

    memmove((char *)arr + i * el_size, items, n * el_size);
    if (i + n + 1 > hdr->len)
        hdr->len = i + n + 1;
    return i;
}

size_t mos_arr_concat_fn(void *p_arr, void *arr2, size_t el_size, size_t init_cap) {
    size_t arr2_len = mos_arr_len(arr2);
    if (arr2_len == 0)
        return -1;

    return mos_arr_append_n_fn(p_arr, arr2, arr2_len, el_size, init_cap);
}

size_t mos_arr_del_fn(void *arr, size_t i, size_t el_size) {
    return mos_arr_del_n_fn(arr, i, 1, el_size);
}

size_t mos_arr_del_n_fn(void *arr, size_t i, size_t n, size_t el_size) {
    if (!arr)
        return 0;

    MosArrHeader *hdr = arr2hdr(arr);
    if (i >= hdr->len)
        return 0;

    size_t new_n = n > hdr->len - i ? hdr->len - i : n;
    memmove((char *)arr + i * el_size, (char *)arr + (i + new_n) * el_size, (hdr->len - i) * el_size);
    hdr->len -= new_n;
    return new_n;
}

size_t mos_arr_del_left_fn(void *arr, size_t n, size_t el_size) {
    return mos_arr_del_n_fn(arr, 0, n, el_size);
}

size_t mos_arr_del_right_fn(void *arr, size_t n) {
    if (!arr)
        return 0;

    MosArrHeader *hdr = arr2hdr(arr);
    size_t new_n = n > hdr->len ? hdr->len : n;
    hdr->len -= new_n;
    return new_n;
}

size_t mos_arr_len(void *arr) {
    if (!arr)
        return 0;
    return arr2hdr(arr)->len;
}

size_t mos_arr_set_len_fn(void *p_arr, size_t len, size_t el_size, size_t init_cap) {
    ini(len);

    size_t del = hdr->len > len ? hdr->len - len : 0;
    if (len > hdr->len)
        memset((char *)arr + hdr->len, 0, (len - hdr->len) * el_size);

    hdr->len = len;
    return del;
}

size_t mos_arr_cap(void *arr) {
    if (!arr)
        return 0;
    return arr2hdr(arr)->cap;
}

size_t mos_arr_set_cap_fn(void *p_arr, size_t cap, size_t el_size) {
    void *arr = *(void **)p_arr;
    if (!arr) {
        *(void **)p_arr = init(cap, el_size);
        return cap;
    }

    MosArrHeader *hdr = arr2hdr(arr);
    hdr->cap = hdr->len > cap ? hdr->len : cap;
    hdr = mos_safe_realloc(hdr, sizeof(MosArrHeader) + hdr->cap * el_size);
    *(void **)p_arr = hdr2arr(hdr);
    return hdr->cap;
}

void mos_arr_free(void *arr) {
    if (!arr)
        return;
    free(arr2hdr(arr));
}
