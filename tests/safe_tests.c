#include "safe_tests.h"
#include "moslib/safe.h"

#include <limits.h>
#include <stdlib.h>

Test malloc_exit() {
    safe_malloc(ULLONG_MAX);
}

Test test_safe_malloc() {
    void *p = safe_malloc(1024);
    test_assert(p != NULL, "Failed to malloc 1KiB");

    suppress_output(stderr);
    test_assert_exit(malloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test realloc_exit() {
    void *p = NULL;
    safe_realloc(p, ULLONG_MAX);
}

Test calloc_exit() {
    safe_calloc(ULLONG_MAX, ULLONG_MAX);
}

Test test_safe_calloc() {
    void *p = safe_calloc(16, 16);
    test_assert(p != NULL, "Failed to calloc 16 items of 16 bytes");

    suppress_output(stderr);
    test_assert_exit(calloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test test_safe_realloc() {
    void *p = NULL;
    p = safe_realloc(p, 1024);
    test_assert(p != NULL, "Failed to realloc NULL to 1KiB");

    p = safe_realloc(p, 4096);
    test_assert(p != NULL, "Failed to realloc 1KiB to 4KiB");

    suppress_output(stderr);
    test_assert_exit(realloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

void safe_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/safe.h");
    add_test(group, test_safe_malloc);
    add_test(group, test_safe_calloc);
    add_test(group, test_safe_realloc);
}
