#define MOS_FORCE_PREFIXES
#include "moslib/safe.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_MESS(...) err_mess(info, __VA_ARGS__)

void err_mess(MosSafeErrInfo info, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "%s:%d - fatal error in function %s:\n", info.file, info.line, info.func);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void *mos_safe_malloc_fn(size_t size, MosSafeErrInfo info) {
    void *p = malloc(size);
    if (p)
        return p;
    ERR_MESS("failed to malloc %zu bytes", size);
    exit(EXIT_FAILURE);
}

void *mos_safe_calloc_fn(size_t count, size_t size, MosSafeErrInfo info) {
    void *p = calloc(count, size);
    if (p)
        return p;
    ERR_MESS("failed to calloc %zu items of %zu size", count, size);
    exit(EXIT_FAILURE);
}

void *mos_safe_realloc_fn(void *ptr, size_t size, MosSafeErrInfo info) {
    void *p = realloc(ptr, size);
    if (p)
        return p;
    ERR_MESS("failed to realloc %p to %zu bytes", ptr, size);
    exit(EXIT_FAILURE);
}

FILE *mos_safe_fopen_fn(const char *path, const char *mode, MosSafeErrInfo info) {
    FILE *p = fopen(path, mode);
    if (p)
        return p;
    ERR_MESS("failed to fopen %s with mode %s", path, mode);
    exit(EXIT_FAILURE);
}

FILE *mos_safe_tmpfile_fn(MosSafeErrInfo info) {
    FILE *p = tmpfile();
    if (p)
        return p;
    ERR_MESS("failed to create a tmpfile");
    exit(EXIT_FAILURE);
}

int mos_safe_dup_fn(int fd, MosSafeErrInfo info) {
    int res = dup(fd);
    if (res != -1)
        return res;
    ERR_MESS("failed to dup %d", fd);
    exit(EXIT_FAILURE);
}

int mos_safe_dup2_fn(int fd, int fd2, MosSafeErrInfo info) {
    int res = dup2(fd, fd2);
    if (res != -1)
        return res;
    ERR_MESS("failed to dup %d 2 %d", fd, fd2);
    exit(EXIT_FAILURE);
}
