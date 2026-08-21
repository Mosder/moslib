// moslib/safe.h
// "Safe" wrappers for some "unsafe" functions
// aka "I don't want to check for NULL every single time" header

#include <stddef.h>
#include <stdio.h>

#ifndef MOSLIB_SAFE_H
#define MOSLIB_SAFE_H

#ifndef MOS_FORCE_PREFIXES

#define safe_malloc mos_safe_malloc
#define safe_calloc mos_safe_calloc
#define safe_realloc mos_safe_realloc
#define safe_fopen mos_safe_fopen
#define safe_tmpfile mos_safe_tmpfile
#define safe_dup mos_safe_dup
#define safe_dup2 mos_safe_dup2

#endif // MOS_FORCE_PREFIXES

typedef struct {
    const char *file;
    int line;
    const char *func;
} MosSafeErrInfo;

#define mos_err_info ((MosSafeErrInfo){.file = __FILE__, .line = __LINE__, .func = __func__})

// All of the below function-like macros do the same thing the original functions do
// They check whether the operation suceeded and if not - they exit with an error message
#define mos_safe_malloc(size) mos_safe_malloc_fn(size, mos_err_info)
#define mos_safe_calloc(count, size) mos_safe_calloc_fn(count, size, mos_err_info)
#define mos_safe_realloc(ptr, size) mos_safe_realloc_fn(ptr, size, mos_err_info)
#define mos_safe_fopen(path, mode) mos_safe_fopen_fn(path, mode, mos_err_info)
#define mos_safe_tmpfile() mos_safe_tmpfile_fn(mos_err_info)
#define mos_safe_dup(fd) mos_safe_dup_fn(fd, mos_err_info)
#define mos_safe_dup2(fd, fd2) mos_safe_dup2_fn(fd, fd2, mos_err_info)

// Function prototypes for macros
extern void *mos_safe_malloc_fn(size_t size, MosSafeErrInfo info);
extern void *mos_safe_calloc_fn(size_t count, size_t size, MosSafeErrInfo info);
extern void *mos_safe_realloc_fn(void *ptr, size_t size, MosSafeErrInfo info);
extern FILE *mos_safe_fopen_fn(const char *path, const char *mode, MosSafeErrInfo info);
extern FILE *mos_safe_tmpfile_fn(MosSafeErrInfo info);
extern int mos_safe_dup_fn(int fd, MosSafeErrInfo info);
extern int mos_safe_dup2_fn(int fd, int fd2, MosSafeErrInfo info);

#endif // MOSLIB_SAFE_H
