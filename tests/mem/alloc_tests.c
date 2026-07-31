#include "alloc_tests.h"

#include <limits.h>
#include <moslib/mem/alloc.h>
#include <stdlib.h>

Test malloc_exit() {
    safe_malloc(ULLONG_MAX);
}

Test test_safe_malloc() {
    void *p = safe_malloc(1024);
    test_assert(p != NULL, "Should be able to malloc 1KiB");
    test_assert_exit(malloc_exit, EXIT_FAILURE);
}

Test realloc_exit() {
    void *p = NULL;
    safe_realloc(p, ULLONG_MAX);
}

Test test_safe_realloc() {
    void *p = NULL;
    p = safe_realloc(p, 1024);
    test_assert(p != NULL, "Should be able to realloc NULL to 1KiB");

    p = safe_realloc(p, 4096);
    test_assert(p != NULL, "Should be able to realloc 1KiB to 4KiB");

    test_assert_exit(realloc_exit, EXIT_FAILURE);
}

void mem_alloc_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/mem/alloc.h");
    add_test(group, test_safe_malloc);
    add_test(group, test_safe_realloc);
}
