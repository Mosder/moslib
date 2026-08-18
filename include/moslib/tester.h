// moslib/tester.h
// Helper functions for writing and running tests

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// Define the tester:
//
//      Tester *tester = NULL;
//
// It will initialize on first operation.
//
// You can also initialize a tester from the get go:
//
//      Tester *tester = new_tester();
//
// ----------------------------------------------------------------------------------------------------
//
// Add test groups:
//
//      TestGroup *test_group = add_test_group(tester, group_name);
//
// ----------------------------------------------------------------------------------------------------
//
// Create tests.
// Tests are of type TestFn (Test (*)()).
// They should use test_assert functions to check correctness.
// Example:
//      Test test_fn() {
//          int result1 = 1 + 2;
//          int result2 = 3 + 5;
//
//          test_assert(result1 == 3, "1 + 2 is not equal 3");
//          test_assert(result2 == 8, "3 + 5 is not equal 8");
//      }
//
// You can also test if a function is supposed to call exit.
// You need to create a helper test and pass it into test_assert_exit in main test.
// Pass the helper test and expected exit code to it.
// Example:
//      Test helper_test() {
//          int param1, param2;
//          int result = exit_failure_function(&param1, &param2);
//      }
//      Test main_test() {
//          test_assert_exit(helper_test, EXIT_FAILURE);
//      }
//
// If your functions have output and you want to suppress them for the sake of tests,
// you can do so using suppress_output and unsupress_outputs.
// Example:
//      Test error_test() {
//          suppress_output(&stdout);
//          suppress_output(&stderr);
//          int code = fun_that_fails_with_output();
//          unsuppress_outputs();
//          test_assert(code == 0, "function didn't fail");
//      }
//
// ----------------------------------------------------------------------------------------------------
//
// Add tests to test group:
//
//      add_test(test_group, test_fn);
//
// ----------------------------------------------------------------------------------------------------
//
// Run tests:
//
//      int all_passed = run_tests(tester);
//
// ----------------------------------------------------------------------------------------------------
//
// Free tester:
//
//      free_tester(tester);
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#include <stdio.h>

#ifndef MOSLIB_TESTER_H
#define MOSLIB_TESTER_H

#ifndef MOS_FORCE_PREFIXES

#define Test MosTest
#define Tester MosTester
#define TestGroup MosTestGroup
#define TestFn MosTestFn
#define new_tester mos_new_tester
#define add_test_group mos_add_test_group
#define test_assert mos_test_assert
#define test_assert_exit mos_test_assert_exit
#define suppress_output mos_suppress_output
#define unsuppress_outputs mos_unsuppress_outputs
#define add_test mos_add_test
#define run_tests mos_run_tests
#define free_tester mos_free_tester

#endif // MOS_FORCE_PREFIXES

typedef void MosTest; // this is just for nice indication that a function is a test
typedef struct MosTester MosTester;
typedef struct MosTestGroup MosTestGroup;
typedef MosTest (*MosTestFn)(void);

// Initalize new tester
// You can skip this and just define it as NULL - it will initalize automatically
//
// Returns:
//   pointer to the new tester
extern MosTester *mos_new_tester(void);

// Add new test group to the tester
//
// Arguments:
//   tester
//     tester to add the group to
//
//   group_name
//     name of the group to add
//
// Returns:
//   pointer to the added group
#define mos_add_test_group(tester, group_name) ((!(tester) ? (tester) = mos_new_tester() : 0), mos_add_test_group_fn(tester, group_name))

// Assert that an expression is true
// Use in test functions to test correctness
//
// Arguments:
//   expression
//     expression to check - if it's true, test passes
//                           if it's false, test fails
//
//   fail_message
//     message to display when test fails
extern void mos_test_assert(int expression, const char *fail_message);

// Assert that function exits with given exit code
//
// Arguments:
//   function
//     function that is supposed to exit
//
//   code
//     expected exit code
#define mos_test_assert_exit(function, code) mos_test_assert_exit_fn(function, code, #function);

// Suppress given output
//
// Arguments:
//   out
//     pointer to outpout to suppress
extern void mos_suppress_output(FILE **out);

// Unuppress previously suppressed outputs
// Will unsupress all of the outputs that were suppressed
extern void mos_unsuppress_outputs(void);

// Add test to a test group
//
// Arguments:
//   group
//     test group to add the test to
//
//   test
//     test to add to the group
#define mos_add_test(group, test) mos_add_test_fn(group, test, #test);

// Run defined tests
//
// Arguments:
//   tester
//     tester with tests to run
//
// Returns:
//   1 if all tests passed, 0 if not
extern int mos_run_tests(MosTester *tester);

// Free the tester
//
// Arguments:
//   tester
//     tester to free
extern void mos_free_tester(MosTester *tester);

// Function prototypes for macros
extern MosTestGroup *mos_add_test_group_fn(MosTester *tester, const char *group_name);
extern void mos_test_assert_exit_fn(MosTestFn function, int code, const char *name);
extern void mos_add_test_fn(MosTestGroup *group, MosTestFn test, const char *name);

#endif // MOSLIB_TESTER_H
