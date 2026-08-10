#include "moslib/tester.h"

#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "moslib/mem/alloc.h"

#define INIT_GROUP_CAP 8
#define INIT_TEST_CAP 16

#define RED 91
#define GREEN 92
#define WHITE 97

#define RENDER_WIDTH 80

int failed = 0;
FILE *dev_null = NULL;

typedef struct {
    FILE ***outs;
    FILE **ogs;
    size_t n_outs;
    size_t out_cap;
} Outputs;

Outputs suppressed = {
    .outs = NULL,
    .ogs = NULL,
    .n_outs = 0,
    .out_cap = 4,
};

#define print_color(template, ...) printf("\033[%dm" template "\033[0m", __VA_ARGS__)
#define nl printf("\n")

void print_center(const char *text, int color, char padding_char) {
    size_t post_char_count = (RENDER_WIDTH - strlen(text)) / 2;
    size_t pre_char_count = RENDER_WIDTH - post_char_count - strlen(text);

    for (size_t i = 1; i < pre_char_count; i++)
        printf("%c", padding_char);

    print_color(" %s ", color, text);

    for (size_t i = 1; i < post_char_count; i++)
        printf("%c", padding_char);

    nl;
}

typedef struct {
    TestFn test;
    const char *name;
} TestName;

struct Tester {
    TestGroup *groups;
    size_t n_groups;
    size_t group_cap;
};

struct TestGroup {
    const char *name;
    TestName *tests;
    size_t n_tests;
    size_t test_cap;
};

Tester *new_tester(void) {
    Tester *tester = safe_malloc(sizeof(Tester));
    *tester = (Tester){
        .groups = safe_malloc(INIT_GROUP_CAP * sizeof(TestGroup)),
        .n_groups = 0,
        .group_cap = INIT_GROUP_CAP,
    };
    return tester;
}

TestGroup *add_test_group_fn(Tester *tester, const char *group_name) {
    if (tester->n_groups >= tester->group_cap) {
        tester->group_cap *= 2;
        tester->groups = safe_realloc(tester->groups, tester->group_cap * sizeof(TestGroup));
    }

    TestGroup *group = tester->groups + tester->n_groups++;
    *group = (TestGroup){
        .name = group_name,
        .tests = safe_malloc(INIT_TEST_CAP * sizeof(TestName)),
        .n_tests = 0,
        .test_cap = INIT_TEST_CAP,
    };
    return group;
}

void test_assert(int expression, const char *fail_message) {
    if (!expression) {
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("%s\n", fail_message);
    }
}

jmp_buf jump_env;
int expected_code;
const char *fn_name;
void exit(int code) {
    if (code != expected_code) {
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("exit code was %d, expected %d (in function %s)\n", code, expected_code, fn_name);
    }
    longjmp(jump_env, 1);
}

void test_assert_exit_fn(TestFn function, int code, const char *name) {
    expected_code = code;
    fn_name = name;
    if (setjmp(jump_env) == 0) {
        function();
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("exit was not called (in function %s)\n", name);
    }
}

void suppress_output(FILE **out) {
    if (!suppressed.outs || suppressed.n_outs >= suppressed.out_cap) {
        while (suppressed.n_outs >= suppressed.out_cap)
            suppressed.out_cap *= 2;
        suppressed.outs = safe_realloc(suppressed.outs, suppressed.out_cap);
        suppressed.ogs = safe_realloc(suppressed.ogs, suppressed.out_cap);
    }
    suppressed.outs[suppressed.n_outs] = out;
    suppressed.ogs[suppressed.n_outs++] = *out;

    if (!dev_null) {
        dev_null = fopen("/dev/null", "w");
        if (!dev_null)
            return;
    }

    *out = dev_null;
}

void unsuppress_outputs(void) {
    if (!suppressed.outs || !suppressed.ogs)
        return;

    for (size_t i = 0; i < suppressed.n_outs; i++)
        *suppressed.outs[i] = suppressed.ogs[i];

    suppressed.n_outs = 0;
}

void add_test_fn(TestGroup *group, TestFn test, const char *name) {
    if (group->n_tests >= group->test_cap) {
        group->test_cap *= 2;
        group->tests = safe_realloc(group->tests, group->test_cap * sizeof(TestName));
    }

    group->tests[group->n_tests++] = (TestName){.test = test, .name = name};
}

void run_tests(Tester *tester) {
    if (!tester)
        return;

    size_t total_passed = 0;
    size_t total_failed = 0;
    char buf[80];

    print_center("START", WHITE, '=');

    for (size_t i = 0; i < tester->n_groups; i++) {
        nl;
        TestGroup *group = tester->groups + i;
        size_t tests_passed = 0;
        size_t tests_failed = 0;

        print_center(group->name, WHITE, '-');

        for (size_t j = 0; j < group->n_tests; j++) {
            failed = 0;
            printf("%s:\n", group->tests[j].name);
            group->tests[j].test();
            if (failed) {
                tests_failed += 1;
                print_color("--- FAILED ---\n", RED);
            }
            else {
                tests_passed += 1;
                print_color("--- PASSED ---\n", GREEN);
            }
        }

        print_center("GROUP SUMMARY", WHITE, '-');

        sprintf(buf, "%zu PASSED", tests_passed);
        print_center(buf, GREEN, ' ');
        sprintf(buf, "%zu FAILED", tests_failed);
        print_center(buf, RED, ' ');
        if (i < tester->n_groups - 1) {
            nl;
            print_center("TESTING", WHITE, '=');
        }

        total_passed += tests_passed;
        total_failed += tests_failed;
    }

    nl;
    print_center("TESTS SUMMARY", WHITE, '=');
    sprintf(buf, "%zu PASSED", total_passed);
    print_center(buf, GREEN, ' ');
    sprintf(buf, "%zu FAILED", total_failed);
    print_center(buf, RED, ' ');
    print_center("END", WHITE, '=');
}

void free_tester(Tester *tester) {
    if (tester) {
        for (size_t i = 0; i < tester->n_groups; i++)
            free(tester->groups[i].tests);
        free(tester->groups);
        free(tester);
    }
    if (dev_null) {
        fclose(dev_null);
    }
    if (suppressed.outs) {
        free(suppressed.outs);
        suppressed.n_outs = 0;
    }
}
