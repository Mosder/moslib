#define _XOPEN_SOURCE 600
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
    arr_append_n(arr, str, 5);
    arr_append_n(arr, str + 5, 8);
    StringSlice ss2 = ss_from_arr(arr);
    test_assert(ss2.data == arr && ss2.len == str_len, "ss_from_arr created wrong slice");

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
    test_assert(ss_eq_str(ss, ss.data), "slice is not equal to its C string");

    char *hw = strdup("hello world");
    StringSlice ss2 = ss_from_str(hw);
    test_assert(ss_eq(ss, ss2), "slice is not equal to an equal slice");
    test_assert(ss_eq_str(ss, hw), "slice is not equal to an equal string");
    free(hw);

    StringSlice ss3 = ss_from_vars(ss.data, ss.len - 1);
    test_assert(ss_eq(ss, ss3) == 0, "slice is equal when the pointer is the same but length differs");

    StringSlice ss4 = ss_from_str("hello");
    StringSlice ss5 = ss_from_str("world");
    test_assert(ss_eq(ss4, ss5) == 0, "slice is equal when the length is the same but pointer differs");
    test_assert(ss_eq_str(ss4, "world") == 0, "slice is equal to different string");
}

Test trimming() {
    StringSlice ss = ss_from_str("   Hi  :)  ");
    StringSlice t = ss_trim(ss);
    test_assert(ss_eq_str(t, "Hi  :)"), "trim didn't trim properly");
    test_assert(ss_eq(t, ss_trim(t)), "trimming slice twice changes output");

    StringSlice tl = ss_trim_left(ss);
    test_assert(ss_eq_str(tl, "Hi  :)  "), "trim_left didn't trim properly");

    StringSlice tr = ss_trim_right(ss);
    test_assert(ss_eq_str(tr, "   Hi  :)"), "trim_right didn't trim properly");

    ss = ss_from_str("     ");
    t = ss_trim(ss);
    test_assert(ss_eq_str(t, ""), "trimming breaks for all whitespace slice");
}

Test prefix_suffix() {
    StringSlice ss = ss_from_str("pre mid suf");

    test_assert(ss_starts_with(ss, ss_from_str("pre")), "slice doesn't start with its slice prefix");
    test_assert(ss_starts_with(ss, ss_from_str("suf")) == 0, "slice starts with not slice prefix");

    test_assert(ss_ends_with(ss, ss_from_str("suf")), "slice doesn't end with its slice suffix");
    test_assert(ss_ends_with(ss, ss_from_str("pre")) == 0, "slice ends with not slice suffix");

    test_assert(ss_starts_with_str(ss, "pre mid"), "slice doesn't start with its string prefix");
    test_assert(ss_starts_with_str(ss, "suf") == 0, "slice starts with not string prefix");

    test_assert(ss_ends_with_str(ss, "mid suf"), "slice doesn't end with its string suffix");
    test_assert(ss_ends_with_str(ss, "pre") == 0, "slice ends with not string suffix");

    StringSlice ss2 = ss_from_str("");
    test_assert(ss_starts_with(ss2, ss_from_str("")), "starts_with breaks for empty slice and prefix");
    test_assert(ss_ends_with(ss2, ss_from_str("")), "ends_with breaks for empty slice and suffix");
}

Test deleting() {
    StringSlice ss = ss_from_str("1234567890");

    size_t deleted = ss_del_left(&ss, 2);
    test_assert(deleted == 2, "ss_del_left returned wrong deleted count");
    test_assert(ss_eq_str(ss, "34567890"), "del_left deleted wrong chars");

    deleted = ss_del_right(&ss, 6);
    test_assert(deleted == 6, "ss_del_right returned wrong deleted count");
    test_assert(ss_eq_str(ss, "34"), "del_right deleted wrong chars");

    deleted = ss_del_left(&ss, 100);
    test_assert(deleted == 2, "ss_del_left returned wrong deleted count (n > len)");
    test_assert(ss_eq_str(ss, ""), "del_left deleted wrong chars (n > len)");

    deleted = ss_del_right(&ss, 15);
    test_assert(deleted == 0, "ss_del_right returned wrong deleted count (n > len)");
    test_assert(ss_eq_str(ss, ""), "del_right deleted wrong chars (n > len)");

    ss = ss_from_str("preprepre sufsufsuf");
    StringSlice pre = ss_from_str("pre");
    StringSlice suf = ss_from_str("suf");

    int flag = ss_del_prefix(&ss, pre);
    test_assert(flag && ss.len == 16, "del_prefix didn't work properly for found prefix");
    flag = ss_del_prefix(&ss, suf);
    test_assert(!flag && ss.len == 16, "del_prefix didn't work properly for not found prefix");

    flag = ss_del_suffix(&ss, suf);
    test_assert(flag && ss.len == 13, "del_suffix didn't work properly for found suffix");
    flag = ss_del_suffix(&ss, pre);
    test_assert(!flag && ss.len == 13, "del_suffix didn't work properly for not found suffix");

    flag = ss_del_prefix_str(&ss, "pre");
    test_assert(flag && ss.len == 10, "del_prefix_str didn't work properly for found prefix");
    flag = ss_del_prefix_str(&ss, "suf");
    test_assert(!flag && ss.len == 10, "del_prefix_str didn't work properly for not found prefix");

    flag = ss_del_suffix_str(&ss, "suf");
    test_assert(flag && ss.len == 7, "del_suffix_str didn't work properly for found suffix");
    flag = ss_del_suffix_str(&ss, "pre");
    test_assert(!flag && ss.len == 7, "del_suffix_str didn't work properly for not found suffix");
}

Test splitting() {
    StringSlice ss1 = ss_from_str("Hello, World!");

    StringSlice ss2 = ss_split(&ss1, ',');
    test_assert(ss_eq_str(ss2, "Hello") && ss_eq_str(ss1, " World!"), "ss_split didn't split proprely");

    StringSlice ss3 = ss_split_str(&ss2, "ll");
    test_assert(ss_eq_str(ss3, "He") && ss_eq_str(ss2, "o"), "ss_split_str didn't split proprely");

    StringSlice ss4 = ss_split_ss(&ss1, SS_LIT("rld"));
    test_assert(ss_eq_str(ss4, " Wo") && ss_eq_str(ss1, "!"), "ss_split_ss didn't split proprely");

    ss1 = ss_from_str("hiii");

    ss2 = ss_split_str(&ss1, "");
    test_assert(ss_eq_str(ss2, "") && ss_eq_str(ss1, "hiii"), "ss_split_str fails empty str edge case");

    ss2 = ss_split_ss(&ss1, SS_LIT(""));
    test_assert(ss_eq_str(ss2, "") && ss_eq_str(ss1, "hiii"), "ss_split_ss fails empty ss edge case");

    ss2 = ss_split(&ss1, '.');
    test_assert(ss_eq_str(ss2, "hiii") && ss_eq_str(ss1, ""), "ss_split fails not found delim edge case");

    ss1 = ss_split_str(&ss2, "..");
    test_assert(ss_eq_str(ss1, "hiii") && ss_eq_str(ss2, ""), "ss_split_str fails not found delim edge case");

    ss2 = ss_split_ss(&ss1, SS_LIT("..."));
    test_assert(ss_eq_str(ss2, "hiii") && ss_eq_str(ss1, ""), "ss_split_ss fails not found delim edge case");

    StringSlice ss_middle = ss_from_str("[left][middle][right]");

    StringSlice ss_left = ss_split_left(&ss_middle, 6);
    test_assert(ss_eq_str(ss_left, "[left]") && ss_eq_str(ss_middle, "[middle][right]"), "ss_split_left didn't split properly");

    StringSlice ss_right = ss_split_right(&ss_middle, 7);
    test_assert(ss_eq_str(ss_right, "[right]") && ss_eq_str(ss_middle, "[middle]"), "ss_split_right didn't split properly");

    ss_left = ss_split_left(&ss_middle, 20);
    test_assert(ss_eq_str(ss_left, "[middle]") && ss_eq_str(ss_middle, ""), "ss_split_left didn't split properly (n > len)");

    ss_right = ss_split_right(&ss_middle, 500);
    test_assert(ss_eq_str(ss_right, "") && ss_eq_str(ss_middle, ""), "ss_split_right didn't split properly (n > len)");
}

Test finding() {
    char *txt = "a slice of FINDME text";
    StringSlice ss = ss_from_str(txt);

    test_assert(ss_eq_str(ss_find(ss, ss_from_str("FINDME")), "FINDME text"), "find didn't find a part of the slice");
    test_assert(ss_eq_str(ss_find_str(ss, "FINDME"), "FINDME text"), "find_str didn't find a part of the slice");

    test_assert(ss_eq(ss_find(ss, ss_from_str("")), ss), "find didn't return whole slice for empty target");
    test_assert(ss_eq(ss_find_str(ss, ""), ss), "find_str didn't return whole slice for empty target");

    test_assert(ss_eq_str(ss_find(ss, ss_from_str("abcd")), ""), "find didn't return empty slice for not found target");
    test_assert(ss_eq_str(ss_find_str(ss, "abcd"), ""), "find_str didn't return empty slice for not found target");
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
