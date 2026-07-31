// moslib/mem/alloc.h
// Helper functions for general memory management

#include <stddef.h>

#ifndef MOSLIB_MEM_ALLOC_H
#define MOSLIB_MEM_ALLOC_H

#define MOSLIB_MAX_ALLOC_ATTEMPTS 3

// Tries to malloc MOSLIB_MAX_ALLOC_ATTEMPTS times
// If all attempts fail - exits with EXIT_FAILURE code
//
// Arguments:
//   size
//     count of bytes to malloc
//
// Returns:
//   pointer to the allocated memory
#define safe_malloc(size) safe_malloc_fn(size, __FILE__, __LINE__, __func__)

// Tries to realloc MOSLIB_MAX_ALLOC_ATTEMPTS times
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
#define safe_realloc(ptr, size) safe_realloc_fn(ptr, size, __FILE__, __LINE__, __func__)

// Function prototypes for macros
extern void *safe_malloc_fn(size_t size, const char *file, int line, const char *func);
extern void *safe_realloc_fn(void *ptr, size_t size, const char *file, int line, const char *func);

#endif // MOSLIB_MEM_ALLOC_H
