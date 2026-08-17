#include "alloc_tests.h"
#define MOS_MAX_ALLOC_ATTEMPTS 1
#include "moslib/mem/alloc.h"

#include <limits.h>
#include <stdlib.h>

Test malloc_exit() {
    safe_malloc(ULLONG_MAX);
}

Test test_safe_malloc() {
    void *p = safe_malloc(1024);
    test_assert(p != NULL, "Failed to malloc 1KiB");

    suppress_output(&stderr);
    test_assert_exit(malloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test realloc_exit() {
    void *p = NULL;
    safe_realloc(p, ULLONG_MAX);
}

Test test_safe_realloc() {
    void *p = NULL;
    p = safe_realloc(p, 1024);
    test_assert(p != NULL, "Failed to realloc NULL to 1KiB");

    p = safe_realloc(p, 4096);
    test_assert(p != NULL, "Failed to realloc 1KiB to 4KiB");

    suppress_output(&stderr);
    test_assert_exit(realloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

void mem_alloc_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/mem/alloc.h");
    add_test(group, test_safe_malloc);
    add_test(group, test_safe_realloc);
}
