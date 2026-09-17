// compat.h
// Compatibility layer for functions not in standard C99

#ifndef MOSLIB_COMPAT_H
#define MOSLIB_COMPAT_H

#define _GNU_SOURCE

typedef void declaration;

#ifdef NO_STRDUP
extern char *strdup(const char *s);
#endif // NO_STRDUP

#ifdef NO_MEMMEM
#include <stddef.h>
extern void *memmem(const void *h, size_t h_size, const void *n, size_t n_size);
#endif // NO_MEMMEM

#endif // MOSLIB_COMPAT_H
