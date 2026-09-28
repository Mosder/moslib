#include "compat.h"

#ifdef NO_STRDUP
#include <stdlib.h>
#include <string.h>
char *mos_strdup(const char *s) {
    char *s2 = malloc(strlen(s) + 1);
    if (!s2)
        return NULL;
    strcpy(s2, s);
    return s2;
}
#endif // NO_STRDUP

#ifdef NO_MEMMEM
void *mos_memmem(const void *h, size_t h_size, const void *n, size_t n_size) {
    if (n_size == 0)
        return (void *)h;
    if (n_size > h_size)
        return NULL;

    const unsigned char *p = h;
    const unsigned char *end = p + h_size - n_size + 1;
    unsigned char c = *(unsigned char *)n;
    while ((p = memchr(p, c, end - p))) {
        if (memcmp(p, n, n_size) == 0)
            return (void *)p;
        p++;
    }

    return NULL;
}
#endif // NO_MEMMEM
