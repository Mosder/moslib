#include "safe_tests.h"
#include "moslib/safe.h"

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

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

    suppress_output(stderr);
    test_assert_exit(realloc_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test fopen_exit() {
    safe_fopen("/non-existing-file-name", "r");
}

Test test_safe_fopen() {
    FILE *fp = safe_fopen(__FILE__, "r");
    test_assert(fp != NULL, "Failed to fopen current file");

    suppress_output(stderr);
    test_assert_exit(fopen_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test test_safe_tmpfile() {
    FILE *fp = safe_tmpfile();
    test_assert(fp != NULL, "Failed to create a tmpfile");
}

Test dup_exit() {
    safe_dup(-1);
}

Test test_safe_dup() {
    int fd = safe_dup(STDOUT_FILENO);
    test_assert(fd != -1, "Failed to dup STDOUT_FILENO");
    close(fd);

    suppress_output(stderr);
    test_assert_exit(dup_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

Test dup2_exit() {
    safe_dup2(-1, STDOUT_FILENO);
}

Test test_safe_dup2() {
    int res = dup2(STDOUT_FILENO, STDOUT_FILENO);
    test_assert(res != -1, "Failed to dup STDOUT_FILENO 2 itself");

    int fd = dup(STDOUT_FILENO);
    res = dup2(STDERR_FILENO, STDOUT_FILENO);
    test_assert(res != -1, "Failed to dup STDERR_FILENO 2 STDOUT_FILENO");
    res = dup2(fd, STDOUT_FILENO);
    test_assert(res != -1, "Failed to restore STDOUT_FILENO");
    close(fd);

    suppress_output(stderr);
    test_assert_exit(dup2_exit, EXIT_FAILURE);
    unsuppress_outputs();
}

void safe_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/safe.h");
    add_test(group, test_safe_malloc);
    add_test(group, test_safe_calloc);
    add_test(group, test_safe_realloc);
    add_test(group, test_safe_fopen);
    add_test(group, test_safe_tmpfile);
    add_test(group, test_safe_dup);
    add_test(group, test_safe_dup2);
}
