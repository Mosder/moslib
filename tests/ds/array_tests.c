#include "array_tests.h"
#include "moslib/ds/array.h"

#include <string.h>

Test cap_test() {
    float *arr = NULL;
    test_assert(arr_cap(arr) == 0, "uninitialized array has capacity different than 0");

    arr_append(arr, 0.0);
    test_assert(arr_cap(arr) == ARR_INIT_CAP, "initial arr capacity is incorrect");

    float items[ARR_INIT_CAP] = {0};
    arr_append_n(arr, items, ARR_INIT_CAP);
    test_assert(arr_cap(arr) == 2 * ARR_INIT_CAP, "capacity didn't increase 2 times");

    size_t new_cap = arr_set_cap(arr, 3 * ARR_INIT_CAP);
    test_assert(new_cap == arr_cap(arr) && new_cap == 3 * ARR_INIT_CAP, "set cap didn't set correct capacity (cap >= len)");

    new_cap = arr_set_cap(arr, 0);
    test_assert(new_cap == arr_cap(arr) && new_cap == ARR_INIT_CAP + 1, "set cap didn't set correct capacity (cap < len)");
}

Test len_test() {
    char *arr = NULL;
    test_assert(arr_len(arr) == 0, "uninitialized array has length different than 0");

    size_t deleted = arr_set_len(arr, 5);
    test_assert(arr_len(arr) == 5, "set_len set wrong length");
    test_assert(deleted == 0, "increasing length deleted items");

    deleted = arr_set_len(arr, 3);
    test_assert(arr_len(arr) == 3, "set_len set wrong length");
    test_assert(deleted == 2, "decreasing length deleted wrong amount of items");
}

Test adding_items() {
    int *arr = NULL;

    // {3}
    size_t index = arr_append(arr, 3);
    test_assert(index == 0, "first appended item didn't return index 0");

    // {3, 8}
    index = arr_push(arr, 8);
    test_assert(index == 1, "second pushed item didn't return index 1");

    // {3, 1, 8}
    index = arr_insert(arr, 1, 1);
    test_assert(index == 1, "item inserted at index <= arr_len didn't return same index");

    // {3, 1, 8, 0, 0, 7}
    index = arr_put(arr, 5, 7);
    test_assert(index == 5, "item put at index > arr_len didn't return proper index");

    // {3, 1, 8, 0, 0, 7, 1, 2}
    int items[] = {1, 2, 3, 4, 5, 6, 7};
    index = arr_append_n(arr, items, 2);
    test_assert(index == 6, "append_n returned wrong index");

    // {3, 1, 8, 0, 0, 7, 1, 2, 3}
    index = arr_push_n(arr, items + 2, 1);
    test_assert(index == 8, "push_n returned wrong index");

    // {4, 5, 3, 1, 8, 0, 0, 7, 1, 2, 3}
    index = arr_insert_n(arr, 0, items + 3, 2);
    test_assert(index == 0, "insert_n returned wrong index");

    // {4, 5, 3, 1, 8, 6, 7, 7, 1, 2, 3}
    index = arr_put_n(arr, 5, items + 5, 2);
    test_assert(index == 5, "put_n returned wrong index");

    size_t len = arr_len(arr);
    test_assert(len == 11, "arr_len after adding all items was incorrect");

    int expected[] = {4, 5, 3, 1, 8, 6, 7, 7, 1, 2, 3};
    test_assert(!memcmp(arr, expected, len * sizeof(int)), "array has wrong value(s) in it");
}

Test getting_items() {
    struct test {
        int f1;
        char f2;
    };

    struct test *arr = NULL;
    test_assert(arr_first(arr) == NULL, "first of empty array isn't null");
    test_assert(arr_last(arr) == NULL, "last of empty array isn't null");

    arr_append(arr, (struct test){0});
    test_assert(arr_first(arr) == arr_last(arr), "first and last are different for array of length 1");

    arr_append(arr, (struct test){1, 1});
    test_assert(arr_first(arr) != arr_last(arr), "first and last are the same for array of length >1");

    struct test *next_first = arr_next(arr, NULL);
    struct test *next_last = arr_next(arr, next_first);
    struct test *next_null = arr_next(arr, next_last);
    struct test *prev_last = arr_prev(arr, NULL);
    struct test *prev_first = arr_prev(arr, prev_last);
    struct test *prev_null = arr_prev(arr, prev_first);
    test_assert(next_first && next_last, "next gave NULL when it shouldn't have");
    test_assert(prev_first && prev_last, "prev gave NULL when it shouldn't have");
    test_assert(next_first == prev_first && next_last == prev_last, "next and prev gave different values for the same element");
    test_assert(!next_null, "next didn't give NULL after running out of items");
    test_assert(!prev_null, "prev didn't give NULL after running out of items");

    test_assert(arr_get(arr, -1) == NULL && arr_get(arr, 2) == NULL, "arr_get didn't give NULL when out of bounds");

    struct test i0 = *arr_get(arr, 0);
    struct test i1 = *arr_get(arr, 1);
    test_assert(!memcmp(&i0, &arr[0], sizeof(struct test)) && !memcmp(&i1, &arr[1], sizeof(struct test)), "arr_get(arr, i) gives different result than arr[i]");
}

Test deleting_items() {
    int *arr = NULL;
    int items[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    arr_append_n(arr, items, 9);

    // {1, 2, 3, 4, 5, 6, 7, 8}
    int i1 = arr_pop(arr);
    test_assert(i1 == 9, "arr_pop returned wrong value");

    // {1, 2, 3, 5, 6, 7, 8}
    size_t deleted = arr_del(arr, 3);
    test_assert(deleted == 1, "failed to delete item in bounds");

    // {1, 8}
    deleted = arr_del_n(arr, 1, 5);
    test_assert(deleted == 5, "del_n deleted wrong number of items");
    test_assert(arr_len(arr) == 2 && arr[0] == 1 && arr[1] == 8, "deleted wrong items");

    deleted = arr_del_front(arr, 10);
    test_assert(deleted == 2, "del_front deleted wrong number of items");
    deleted = arr_del_back(arr, 10);
    test_assert(deleted == 0, "del_back deleted wrong number of items");

    arr_append_n(arr, items, 9);
    arr_del_front(arr, 2);
    arr_del_back(arr, 6);
    test_assert(arr_len(arr) == 1 && arr[0] == 3, "wrong array after deletions");
}

Test arr_concat_test() {
    unsigned long *arr1 = NULL;
    unsigned long *arr2 = NULL;

    size_t index = arr_concat(arr1, arr2);
    test_assert(index == (size_t)-1, "arr_concat with arr2 as NULL didn't return -1");

    arr_set_len(arr2, 0);
    index = arr_concat(arr1, arr2);
    test_assert(index == (size_t)-1, "arr_concat with 0 length arr2 didn't return -1");

    unsigned long items[] = {1, 2, 3, 4, 5};
    arr_append_n(arr1, items, 3);
    arr_append_n(arr2, items + 3, 2);

    index = arr_concat(arr1, arr2);
    test_assert(index == 3, "arr_concat returned wrong index");
    test_assert(arr_len(arr1) == 5, "arr_concat produced array of wrong length");
    test_assert(!memcmp(arr1, items, 5 * sizeof(unsigned long)), "arr_concat produced wrong array");

    index = arr_concat(arr2, arr1);
    test_assert(index == 2, "arr_concat returned wrong index");
    unsigned long expected[] = {4, 5, 1, 2, 3, 4, 5};
    test_assert(arr_len(arr2) == 7, "arr_concat produced array of wrong length");
    test_assert(!memcmp(arr2, expected, 7 * sizeof(unsigned long)), "arr_concat produced wrong array");
}

void ds_array_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/ds/array.h");
    add_test(group, cap_test);
    add_test(group, len_test);
    add_test(group, adding_items);
    add_test(group, getting_items);
    add_test(group, deleting_items);
    add_test(group, arr_concat_test);
}
