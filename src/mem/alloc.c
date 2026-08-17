#define MOS_FORCE_PREFIXES
#include "moslib/mem/alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_MESS(fl, ln, fn, msg) "%s:%d - error in function %s:\n%s\n", fl, ln, fn, msg

void *mos_safe_malloc_fn(size_t size, size_t att, const char *file, int line, const char *func) {
    for (size_t i = 0; i < att; i++) {
        void *p = malloc(size);
        if (p)
            return p;
        if (i < att - 1) {
            fprintf(stderr, ERR_MESS(file, line, func, "malloc failed - retrying in 1s..."));
            sleep(1);
        }
    }

    char errmes[64];
    sprintf(errmes, "FATAL: malloc failed %zu times - exiting...", att);
    fprintf(stderr, ERR_MESS(file, line, func, errmes));
    exit(EXIT_FAILURE);
}

void *mos_safe_realloc_fn(void *ptr, size_t size, size_t att, const char *file, int line, const char *func) {
    if (!ptr)
        return mos_safe_malloc_fn(size, att, file, line, func);

    for (size_t i = 0; i < att; i++) {
        void *p = realloc(ptr, size);
        if (p)
            return p;
        if (i < att - 1) {
            fprintf(stderr, ERR_MESS(file, line, func, "realloc failed - retrying in 1s..."));
            sleep(1);
        }
    }

    char errmes[64];
    sprintf(errmes, "FATAL: realloc failed %zu times - exiting...", att);
    fprintf(stderr, ERR_MESS(file, line, func, errmes));
    exit(EXIT_FAILURE);
}
