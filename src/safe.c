#define MOS_FORCE_PREFIXES
#include "moslib/safe.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_MESS(msg, ...) err_mess(info, msg, __VA_ARGS__)

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
