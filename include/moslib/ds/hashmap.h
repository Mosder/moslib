// moslib/ds/hashmap.h
// Implementation of hashmap

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#include <stddef.h>
#include <stdint.h>

#ifndef MOSLIB_DS_HASHMAP_H
#define MOSLIB_DS_HASHMAP_H

// Initial capacity to use in the hashmap
#ifndef MOS_HM_INIT_CAP
#define MOS_HM_INIT_CAP 16
#endif // MOS_HM_INIT_CAP

#ifndef MOS_FORCE_PREFIXES

#define Hashmap MosHashmap
#define hm_new mos_hm_new

#endif // MOS_FORCE_PREFIXES

typedef struct MosHashmap MosHashmap;

typedef struct {
    enum { DEFAULT, STR, SS } key;
    uint32_t (*hash)(void *key);
    int (*eq)(void *key1, void *key2);
} MosHmInitArgs;

// Create a new hashmap
//
// Arguments:
//   key_t
//     type of key to use
//
//   val_t
//     type of value to use
//
// Optional arguments:
//   .key
//     key specification enum
//     STR - C string hash and equals functions
//     SS  - StringSlice hash and equals functions
//
//   .hash
//     user defined hash function
//     will override the one specified by .key
//
//   .eq
//     user defined equals function
//     will override the one specified by .key
//
// Returns:
//   pointer to the new hashmap
#define mos_hm_new(key_t, val_t, ...) mos_hm_new_fn(sizeof(key_t), sizeof(val_t), (MosHmInitArgs){.key = DEFAULT, __VA_ARGS__})

// Function prototypes for macros
extern MosHashmap *mos_hm_new_fn(size_t key_size, size_t val_size, MosHmInitArgs args);

#endif // MOSLIB_DS_HASHMAP_H
