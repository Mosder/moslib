#define MOS_FORCE_PREFIXES
#include "moslib/tester.h"

#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "moslib/safe.h"

#define INIT_GROUP_CAP 8
#define INIT_TEST_CAP 16

#define RED 91
#define GREEN 92
#define WHITE 97

#define RENDER_WIDTH 80

static int failed = 0;
static FILE *dev_null = NULL;

typedef struct {
    FILE ***outs;
    FILE **ogs;
    size_t n_outs;
    size_t out_cap;
} Outputs;

static Outputs suppressed = {
    .outs = NULL,
    .ogs = NULL,
    .n_outs = 0,
    .out_cap = 4,
};

#define print_color(template, ...) printf("\033[%dm" template "\033[0m", __VA_ARGS__)
#define nl printf("\n")

static void print_center(const char *text, int color, char padding_char) {
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
    MosTestFn test;
    const char *name;
} TestName;

struct MosTester {
    MosTestGroup *groups;
    size_t n_groups;
    size_t group_cap;
};

struct MosTestGroup {
    const char *name;
    TestName *tests;
    size_t n_tests;
    size_t test_cap;
};

MosTester *mos_new_tester(void) {
    MosTester *tester = mos_safe_malloc(sizeof(MosTester));
    *tester = (MosTester){
        .groups = mos_safe_malloc(INIT_GROUP_CAP * sizeof(MosTestGroup)),
        .n_groups = 0,
        .group_cap = INIT_GROUP_CAP,
    };
    return tester;
}

MosTestGroup *mos_add_test_group_fn(MosTester *tester, const char *group_name) {
    if (tester->n_groups >= tester->group_cap) {
        tester->group_cap *= 2;
        tester->groups = mos_safe_realloc(tester->groups, tester->group_cap * sizeof(MosTestGroup));
    }

    MosTestGroup *group = tester->groups + tester->n_groups++;
    *group = (MosTestGroup){
        .name = group_name,
        .tests = mos_safe_malloc(INIT_TEST_CAP * sizeof(TestName)),
        .n_tests = 0,
        .test_cap = INIT_TEST_CAP,
    };
    return group;
}

void mos_test_assert(int expression, const char *fail_message) {
    if (!expression) {
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("%s\n", fail_message);
    }
}

static jmp_buf jump_env;
static int expected_code;
static const char *fn_name;
void exit(int code) {
    if (code != expected_code) {
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("exit code was %d, expected %d (in function %s)\n", code, expected_code, fn_name);
    }
    longjmp(jump_env, 1);
}

void mos_test_assert_exit_fn(MosTestFn function, int code, const char *name) {
    expected_code = code;
    fn_name = name;
    if (setjmp(jump_env) == 0) {
        function();
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("exit was not called (in function %s)\n", name);
    }
}

void mos_test_assert_out_fn(
    MosTestFn function,
    FILE **stream,
    const char *output,
    const char *fn_name,
    const char *stream_name
) {
    size_t n = strlen(output);
    char *out = mos_safe_calloc(n + 2, 1);

    FILE *og = *stream;
    fflush(*stream);
    *stream = mos_safe_tmpfile();

    function();
    fflush(*stream);
    rewind(*stream);
    fread(out, 1, n + 1, *stream);
    fclose(*stream);
    *stream = og;

    if (strcmp(out, output)) {
        failed = 1;
        print_color("ASSERT FAILED: ", RED);
        printf("function %s outputed to %s:\n", fn_name, stream_name);
        printf("               %s (capped at %zu chars)\n", out, n + 1);
        printf("               expected: %s\n", output);
    }
    free(out);
}

void mos_suppress_output_fn(FILE **out) {
    if (!suppressed.outs || suppressed.n_outs >= suppressed.out_cap) {
        while (suppressed.n_outs >= suppressed.out_cap)
            suppressed.out_cap *= 2;
        suppressed.outs = mos_safe_realloc(suppressed.outs, suppressed.out_cap * sizeof(int));
        suppressed.ogs = mos_safe_realloc(suppressed.ogs, suppressed.out_cap * sizeof(int));
    }
    suppressed.outs[suppressed.n_outs] = out;
    suppressed.ogs[suppressed.n_outs++] = *out;

    if (!dev_null)
        dev_null = mos_safe_tmpfile();

    fflush(*out);
    *out = dev_null;
}

void mos_unsuppress_outputs(void) {
    if (!suppressed.outs || !suppressed.ogs)
        return;

    if (dev_null)
        fflush(dev_null);

    for (size_t i = suppressed.n_outs; i-- > 0;)
        *suppressed.outs = suppressed.ogs;

    suppressed.n_outs = 0;
}

void mos_add_test_fn(MosTestGroup *group, MosTestFn test, const char *name) {
    if (group->n_tests >= group->test_cap) {
        group->test_cap *= 2;
        group->tests = mos_safe_realloc(group->tests, group->test_cap * sizeof(TestName));
    }

    group->tests[group->n_tests++] = (TestName){.test = test, .name = name};
}

int mos_run_tests(MosTester *tester) {
    if (!tester)
        return 0;

    size_t total_passed = 0;
    size_t total_failed = 0;
    char buf[80];

    print_center("START", WHITE, '=');

    for (size_t i = 0; i < tester->n_groups; i++) {
        nl;
        MosTestGroup *group = tester->groups + i;
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

    return total_failed == 0;
}

void mos_free_tester(MosTester *tester) {
    if (tester) {
        for (size_t i = 0; i < tester->n_groups; i++)
            free(tester->groups[i].tests);
        free(tester->groups);
        free(tester);
    }
    mos_unsuppress_outputs();
    if (dev_null) {
        fclose(dev_null);
        dev_null = NULL;
    }
    if (suppressed.outs) {
        free(suppressed.outs);
        free(suppressed.ogs);
        suppressed.n_outs = 0;
    }
}
