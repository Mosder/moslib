#include "string_tests.h"
#include "moslib/string.h"

#include <stdlib.h>
#include <string.h>

#include "moslib/ds/array.h"

#define HELLO_WORLD_LIT "Hello, World!"

Test creating() {
    char *str = HELLO_WORLD_LIT;
    size_t str_len = strlen(str);

    StringSlice ss1 = ss_from_str(str);
    test_assert(ss1.data == str && ss1.len == str_len, "ss_from_str created wrong slice");

    char *arr = NULL;
    arr_append_n(arr, str, 13);
    StringSlice ss2 = ss_from_arr(arr);
    test_assert(ss2.data == str && ss2.len == str_len, "ss_from_arr created wrong slice");

    StringSlice ss3 = ss_from_vars(str, 13);
    test_assert(ss3.data == str && ss3.len == str_len, "ss_from_vars created wrong slice");

    StringSlice ss4 = SS_LIT(HELLO_WORLD_LIT);
    test_assert(ss4.data == str && ss4.len == sizeof(HELLO_WORLD_LIT) - 1, "SS_LIT created wrong slice");

    StringSlice ss5 = ss_copy(ss1);
    test_assert(ss5.data == ss1.data && ss5.len == ss1.len, "ss_from_arr created wrong slice");
}

Test ss_print_helper() {
    StringSlice ss = ss_from_str("test");
    ss_print(ss);
}

Test printing() {
    test_assert_out(ss_print_helper, stdout, "test");

    StringSlice ss = ss_from_str("slice");
    char buf[16];
    sprintf(buf, "out: " SS_FMT " :)", SS_ARGS(ss));
    test_assert(strcmp(buf, "out: slice :)") == 0, "SS_FMT + SS_ARGS printed wrong output");
}

Test comparing() {
    StringSlice ss = ss_from_str("hello world");
    test_assert(ss_eq(ss, ss), "slice is not equal to itself");

    char *hw = strdup("hello world");
    StringSlice ss2 = ss_from_str(hw);
    test_assert(ss_eq(ss, ss2), "slice is not equal to an equal slice");
    free(hw);

    StringSlice ss3 = ss_from_vars(ss.data, ss.len - 1);
    test_assert(ss_eq(ss, ss3) == 0, "slice is equal when the pointer is the same but length differs");

    StringSlice ss4 = ss_from_str("hello");
    StringSlice ss5 = ss_from_str("world");
    test_assert(ss_eq(ss4, ss5) == 0, "slice is equal when the length is the same but pointer differs");
}

Test trimming() {
    StringSlice ss = ss_from_str("   Hi  :)  ");
    StringSlice t = ss_trim(ss);
    test_assert(ss_eq(t, ss_from_str("Hi  :)")), "ss_trim didn't trim properly");
    test_assert(ss_eq(t, ss_trim(t)), "trimming slice twice changes output");

    StringSlice tl = ss_trim_left(ss);
    test_assert(ss_eq(tl, ss_from_str("Hi  :)  ")), "ss_trim_left didn't trim properly");

    StringSlice tr = ss_trim_right(ss);
    test_assert(ss_eq(tr, ss_from_str("   Hi  :)")), "ss_trim_right didn't trim properly");

    ss = ss_from_str("     ");
    t = ss_trim(ss);
    test_assert(ss_eq(t, ss_from_str("")), "trimming breaks for all whitespace slice");
}

Test prefix_suffix() {
    StringSlice ss = ss_from_str("pre mid suf");

    test_assert(ss_starts_with(ss, ss_from_str("pre")), "slice doesn't start with its prefix");
    test_assert(ss_starts_with(ss, ss_from_str("suf")) == 0, "slice starts with not prefix");

    test_assert(ss_ends_with(ss, ss_from_str("suf")), "slice doesn't end with its suffix");
    test_assert(ss_ends_with(ss, ss_from_str("pre")) == 0, "slice ends with not suffix");

    test_assert(ss_starts_with_str(ss, "pre mid"), "slice doesn't start with its prefix");
    test_assert(ss_starts_with_str(ss, "suf") == 0, "slice starts with not prefix");

    test_assert(ss_ends_with_str(ss, "mid suf"), "slice doesn't end with its suffix");
    test_assert(ss_ends_with_str(ss, "pre") == 0, "slice ends with not suffix");

    StringSlice ss2 = ss_from_str("");
    test_assert(ss_starts_with(ss2, ss_from_str("")), "starts_with breaks for empty slice and prefix");
    test_assert(ss_ends_with(ss2, ss_from_str("")), "ends_with breaks for empty slice and suffix");
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
    StringSlice ss = ss_from_str("1 byte chars");
    test_assert(ss_utf8_len(ss) == ss.len, "utf8_len differs from len for all 1 byte characters");

    ss = ss_from_str("żółw");
    test_assert(ss_utf8_len(ss) == 4, "utf8_len doesn't work for 2 byte characters");

    ss = ss_from_str("ッにプ");
    test_assert(ss_utf8_len(ss) == 3, "utf8_len doesn't work for 3 byte characters");

    ss = ss_from_str("𒀀𓀂");
    test_assert(ss_utf8_len(ss) == 2, "utf8_len doesn't work for 4 byte characters");
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
