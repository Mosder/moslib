#define MOS_FORCE_PREFIXES
#include "moslib/tester.h"

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
static int dev_null = -1;

typedef struct {
    int *outs;
    int *ogs;
    size_t n_outs;
    size_t out_cap;
} Outputs;

static Outputs suppressed = {
    .outs = NULL,
    .ogs = NULL,
    .n_outs = 0,
    .out_cap = 4,
};

#define print_color(color, ...)                                                                    \
    (isatty(STDOUT_FILENO) ? (printf("\033[%dm", color), printf(__VA_ARGS__), printf("\033[0m"))   \
                           : printf(__VA_ARGS__))
#define nl printf("\n")

static void print_center(const char *text, int color, char padding_char) {
    size_t post_char_count = (RENDER_WIDTH - strlen(text)) / 2;
    size_t pre_char_count = RENDER_WIDTH - post_char_count - strlen(text);

    for (size_t i = 1; i < pre_char_count; i++)
        printf("%c", padding_char);

    print_color(color, " %s ", text);

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
        print_color(RED, "ASSERT FAILED: ");
        printf("%s\n", fail_message);
    }
}

void mos_test_assert_exit_fn(MosTestFn function, int code, const char *name) {
    int fd[2];
    mos_safe_pipe(fd);
    pid_t pid = mos_safe_fork();
    if (pid == 0) {
        function();
        uint8_t exited = 0;
        write(fd[1], &exited, sizeof(uint8_t));
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    int exit_code = status >> 8;

    uint8_t exited = 1;
    close(fd[1]);
    read(fd[0], &exited, sizeof(uint8_t));
    close(fd[0]);

    if (!exited) {
        failed = 1;
        print_color(RED, "ASSERT FAILED: ");
        printf("exit was not called (in function %s)\n", name);
    }
    else if (exit_code != code) {
        failed = 1;
        print_color(RED, "ASSERT FAILED: ");
        printf("exit code was %d, expected %d (in function %s)\n", exit_code, code, name);
    }
}

void mos_test_assert_out_fn(
    MosTestFn function, int fd, const char *output, const char *fn_name, const char *fd_name
) {
    size_t n = strlen(output);
    char *out = mos_safe_calloc(n + 2, 1);

    int tmp = mos_safe_mkstemp(".tmp");
    unlink(".tmp");

    int og = mos_safe_dup(fd);
    fflush(NULL);
    mos_safe_dup2(tmp, fd);

    function();
    fflush(NULL);
    lseek(fd, 0, SEEK_SET);
    read(fd, out, n + 1);
    close(tmp);

    mos_safe_dup2(og, fd);
    close(og);

    if (strcmp(out, output)) {
        failed = 1;
        print_color(RED, "ASSERT FAILED: ");
        printf("function %s outputed to %s:\n", fn_name, fd_name);
        printf("               %s (capped at %zu chars)\n", out, n + 1);
        printf("               expected: %s\n", output);
    }
    free(out);
}

void mos_suppress_output_fn(int fd) {
    if (!suppressed.outs || suppressed.n_outs >= suppressed.out_cap) {
        while (suppressed.n_outs >= suppressed.out_cap)
            suppressed.out_cap *= 2;
        suppressed.outs = mos_safe_realloc(suppressed.outs, suppressed.out_cap * sizeof(int));
        suppressed.ogs = mos_safe_realloc(suppressed.ogs, suppressed.out_cap * sizeof(int));
    }
    suppressed.outs[suppressed.n_outs] = fd;
    suppressed.ogs[suppressed.n_outs++] = mos_safe_dup(fd);

    if (dev_null == -1) {
        dev_null = mos_safe_mkstemp(".dev_null");
        unlink(".dev_null");
    }

    fflush(NULL);
    mos_safe_dup2(dev_null, fd);
}

void mos_unsuppress_outputs(void) {
    if (!suppressed.outs || !suppressed.ogs)
        return;

    if (dev_null != -1)
        fflush(NULL);

    for (size_t i = suppressed.n_outs; i-- > 0;) {
        mos_safe_dup2(suppressed.ogs[i], suppressed.outs[i]);
        close(suppressed.ogs[i]);
    }

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
                print_color(RED, "--- FAILED ---\n");
            }
            else {
                tests_passed += 1;
                print_color(GREEN, "--- PASSED ---\n");
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
    if (dev_null != -1) {
        close(dev_null);
        dev_null = -1;
    }
    if (suppressed.outs) {
        free(suppressed.outs);
        free(suppressed.ogs);
        suppressed.n_outs = 0;
    }
}
