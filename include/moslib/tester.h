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
//
// Example:
//      Test test_fn() {
//          int result1 = 1 + 2;
//          int result2 = 3 + 5;
//
//          test_assert(result1 == 3, "1 + 2 should equal 3");
//          test_assert(result2 == 8, "3 + 5 should equal 8");
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
//      run_tests(tester);
//
// ----------------------------------------------------------------------------------------------------
//
// Free tester:
//
//      free_tester(tester);
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#ifndef MOSLIB_TESTER_H
#define MOSLIB_TESTER_H

typedef void Test; // this is just for nice indication that a function is a test
typedef struct Tester Tester;
typedef struct TestGroup TestGroup;
typedef Test (*TestFn)();

#endif // MOSLIB_TESTER_H
