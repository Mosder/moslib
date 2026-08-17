// moslib/mem/alloc.h
// Helper functions for general memory management

#include <stddef.h>

#ifndef MOSLIB_MEM_ALLOC_H
#define MOSLIB_MEM_ALLOC_H

// Attempt count for alloc functions
#ifndef MOS_MAX_ALLOC_ATTEMPTS
#define MOS_MAX_ALLOC_ATTEMPTS 3
#endif // MOS_MAX_ALLOC_ATTEMPTS

#ifndef MOS_FORCE_PREFIXES

#define safe_malloc mos_safe_malloc
#define safe_realloc mos_safe_realloc

#endif // MOS_FORCE_PREFIXES

// Tries to malloc MOS_MAX_ALLOC_ATTEMPTS times
// If all attempts fail - exits with EXIT_FAILURE code
//
// Arguments:
//   size
//     count of bytes to malloc
//
// Returns:
//   pointer to the allocated memory
#define mos_safe_malloc(size) mos_safe_malloc_fn(size, MOS_MAX_ALLOC_ATTEMPTS, __FILE__, __LINE__, __func__)

// Tries to realloc MOS_MAX_ALLOC_ATTEMPTS times
// If all attempts fail - exits with EXIT_FAILURE code
//
// Arguments:
//   ptr
//     pointer to the memory to realloc
//
//   size
//     new desired count of bytes of memory
//
// Returns:
//   pointer to the reallocated memory
#define mos_safe_realloc(ptr, size) mos_safe_realloc_fn(ptr, size, MOS_MAX_ALLOC_ATTEMPTS, __FILE__, __LINE__, __func__)

// Function prototypes for macros
extern void *mos_safe_malloc_fn(size_t size, size_t att, const char *file, int line, const char *func);
extern void *mos_safe_realloc_fn(void *ptr, size_t size, size_t att, const char *file, int line, const char *func);

#endif // MOSLIB_MEM_ALLOC_H
