// moslib/tester.h
// Helper functions for writing and running tests

#ifndef MOSLIB_TESTER_H
#define MOSLIB_TESTER_H

typedef void Test; // this is just for nice indication that a function is a test
typedef struct Tester Tester;
typedef struct TestGroup TestGroup;
typedef Test (*TestFn)(TestGroup *);

// Tester *tester = NULL; (new_tester())
// TestGroup *test_group = add_test_group(Tester *tester, const char *group_name);
// add_test(TestGroup *test_group, TestFn test);
// test_assert(int expression, const char *error_message);
// run_tests(Tester *tester);
// free_tester(Tester *tester);

#endif // MOSLIB_TESTER_H
