// moslib/ds/hashmap.h
// Implementation of hashmap

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// NOTE: quick temporary note about how this should be implemented - to be changed ofc
//
//      struct Entry {
//          <type> key;
//          <type> val;
//      };
//      can use:
//      HM_DEF(name, tk, tv) -> expands to: typedef struct {tk key; tv val;} name
//
//      Struct needs to have these two values. Ig it could have more and it would work if you just use
//      the entry functions, but would have uninitialized values for hm_put as that one will only
//      assign .key and .val fields.
//
//      Initialization:
//
//      Entry *hm = NULL;
//      or
//      Entry *hm = hm_new(opt_args);
//
//      Don't need to do hm_new - only needed when wanting to change hashing / equals functions
//      from the default byte by byte versions (strings, slices, pointers in general y'know)
//      hm_new only mallocs and sets the header, since it doesn't know key / val size
//      on first operation it will allocate init cap, since then it knows sizeof
//
//      will be open addressing with linear probing most certainly, no deleting for now as I don't
//      have a use case for now and it'd add a lot of complexity
//
//      needs a sentinel at index 0 which is memset to 0, to use it for hm_get when key doesn't exist
//      should allocate cap + 1 then, so cap means cap of actual data - not data + sentinel, then
//      (hash & cap) + 1 gives index, sans collisions (cap = 2^n - I don't feel the need to add user
//      set capacity, so that's fine I think)
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#include <stddef.h>
#include <stdint.h>

#ifndef MOSLIB_DS_HASHMAP_H
#define MOSLIB_DS_HASHMAP_H

#ifndef MOS_FORCE_PREFIXES

#define HM_DEF MOS_HM_DEF
#define hm_new mos_hm_new
#define hm_put mos_hm_put
#define hm_put_e mos_hm_put_e
#define hm_get mos_hm_get
#define hm_get_e mos_hm_get_e
#define hm_first mos_hm_first
#define hm_next mos_hm_next
#define hm_free mos_hm_free

#endif // MOS_FORCE_PREFIXES

typedef struct {
    enum { DEFAULT, STR, SS } key;
    uint32_t (*hash)(void *key);
    int (*eq)(void *key1, void *key2);
} MosHmInitArgs;

// clang-format off
// Macro to define Entry struct easier
#define MOS_HM_DEF(name, key_t, val_t) typedef struct {key_t key; val_t val;} name
// clang-format on

// Create a new hashmap
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
#define mos_hm_new(...) mos_hm_new_fn((MosHmInitArgs){__VA_ARGS__})

// Put given value with given key in the hashmap
// If entry of the same key exists - overwrites it
//
// Arguments:
//   hm
//     hashmap to put into
//
//   key
//     key of the entry to put
//
//   ...
//     value of the entry to put
//     variadic to make things like (struct Val){1,1} work properly
#define mos_hm_put(hm, key, ...)

// Put a given entry in the hashmap
// If entry of the same key exists - overwrites it
//
// Arguments:
//   hm
//     hashmap to put into
//
//   ...
//     entry to put into hashmap
//     variadic to make things like (struct Entry){"key","val"} work properly
#define mos_hm_put_e(hm, ...)

// Get the value of entry with specific key
//
// Arguments:
//   hm
//     hashmap to get value from
//
//   key
//     key of the entry to get the value of
//
// Returns:
//   value of the entry with given key or 0 if no such entry exist
#define mos_hm_get(hm, key) 0

// Get a pointer to an entry with specific key in hashmap
//
// Arguments:
//   hm
//     hashmap to get an entry from
//
//   key
//     key of the entry to get
//
// Returns:
//   pointer to the entry or NULL if no entry with such key exists
#define mos_hm_get_e(hm, key) NULL

// Get a pointer to the first entry in the hashmap
//
// Arguments:
//   hm
//     hashmap to get the first entry from
//
// Returns:
//   pointer to the first entry or NULL if hashmap is empty
#define mos_hm_first(hm) NULL

// Get the pointer to the next entry in the hashmap from the pointer to the current one
// If pointer is NULL - gets the first entry from the hashmap
//
// Arguments:
//   hm
//     hashmap to get next entry from
//
//   curr
//     pointer to the current entry
//
// Returns:
//   pointer to the next entry or NULL if there's no more entries
#define mos_hm_next(hm, curr) NULL

// Free the hashmap
//
// Arguments:
//   hm
//     hashmap to free
extern void mos_hm_free(void *hm);

// Function prototypes for macros
extern void *mos_hm_new_fn(MosHmInitArgs args);

#endif // MOSLIB_DS_HASHMAP_H
