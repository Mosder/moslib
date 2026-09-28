// compat.h
// Compatibility layer for functions not in standard C99

#ifndef MOSLIB_COMPAT_H
#define MOSLIB_COMPAT_H

#define _GNU_SOURCE

// In C99 TU needs a declaration (which won't be the case if all functions are available)
typedef void mos_compat_decl;

#ifndef NO_STRDUP
#define mos_strdup strdup
#else
extern char *mos_strdup(const char *s);
#endif // NO_STRDUP

#ifndef NO_MEMMEM
#define mos_memmem memmem
#else
#include <stddef.h>
extern void *mos_memmem(const void *h, size_t h_size, const void *n, size_t n_size);
#endif // NO_MEMMEM

#endif // MOSLIB_COMPAT_H
