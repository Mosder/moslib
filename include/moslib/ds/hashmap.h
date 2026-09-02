// moslib/ds/hashmap.h
// Implementation of hashmap

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// Start by defining an Entry struct:
//
//      struct Entry {
//          <type> key;
//          <type> val;
//      };
//
// The fields of the struct have to be named "key" and "val".
//
// For simple cases you can use the given macro:
//
//      #define MOS_HM_DEF(name, key_t, val_t) typedef struct {key_t key; val_t val;} name
//
// Then you simply define hashmap as:
//
//      struct Entry *hm = NULL;
//
// You can also redefine the default byte-by-byte hash and/or equals functions by using hm_new:
//
//      struct Entry *hm = hm_new(opt_args);
//
// It accepts these optional arguments:
//
//      .key (enum {DEFAULT, STR, SS})        - use predefined hash and equals functions
//                                              for C strings or StringSlice
//      .hash (uint32_t (*)(void *key))       - user defined hash function
//      .eq (int (*)(void *key1, void *key2)) - user defined equals function
//                                              should return 1 if keys are equal, 0 otherwise
//
// .hash and .eq will overwrite the functions set by .key.
//
// It's better to assign NULL than to call hm_new with no arguments when you don't redefine anything.
//
// Examples:
//
//      HM_DEF(str_int, char *, int);
//      str_int *hm1 = hm_new(.key = STR);
//
//      HM_DEF(ss_float, StringSlice, float);
//      ss_float *hm2 = hm_new(.key = SS);
//
//      uint32_t hash_int(void *key) {
//          return (uint32_t)(*(int *)key);
//      }
//      HM_DEF(int_int, int, int);
//      int_int *hm3 = hm_new(.hash = hash_int);
//
// ----------------------------------------------------------------------------------------------------
//
// To put a key/value pair or an entry into a hashmap use:
//
//      hm_put(hm, key, val);
//      hm_put_e(hm, entry);
//
// If an entry with the same key already exists - it will be overwritten.
//
// ----------------------------------------------------------------------------------------------------
//
// To get a value from a hashmap use:
//
//      val_t val = hm_get(hm, key);
//
// If an entry of given key is not in the hashmap, it will return a value with all bytes set to 0.
//
// You can also get a pointer to an entire entry in the hashmap:
//
//      struct Entry *entry = hm_get_e(hm, key);
//
// If an entry of given key is not in the hashmap it will return NULL.
//
// ----------------------------------------------------------------------------------------------------
//
// You can loop over the hashmap using these functions:
//
//      struct Entry *first = hm_first(hm);
//      struct Entry *next = hm_next(hm, curr);
//
// hm_first will return the pointer to the "first entry" (with the lowest hash) in the hashmap.
// If hashmap is empty - it will return NULL.
//
// hm_next returns the pointer to the next entry in the hashmap from the pointer to the current one.
// If hashmap is empty        - it will return NULL.
// If curr is NULL            - same as hm_first.
// If curr is the last entry  - it will return NULL.
//
// ----------------------------------------------------------------------------------------------------
//
// Free the hashmap with:
//
//      hm_free(hm);
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
