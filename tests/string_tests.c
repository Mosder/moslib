#include "string_tests.h"
#include "moslib/string.h"

Test creating() {
    test_assert(0, "TODO");
}

Test printing() {
    test_assert(0, "TODO");
}

Test comparing() {
    test_assert(0, "TODO");
}

Test trimming() {
    test_assert(0, "TODO");
}

Test prefix_suffix() {
    test_assert(0, "TODO");
}

Test deleting() {
    test_assert(0, "TODO");
}

Test splitting() {
    test_assert(0, "TODO");
}

Test finding() {
    test_assert(0, "TODO");
}

Test utf8_len_test() {
    test_assert(0, "TODO");
}

void string_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/string.h");
    add_test(group, creating);
    add_test(group, printing);
    add_test(group, comparing);
    add_test(group, trimming);
    add_test(group, prefix_suffix);
    add_test(group, deleting);
    add_test(group, splitting);
    add_test(group, finding);
    add_test(group, utf8_len_test);
}
