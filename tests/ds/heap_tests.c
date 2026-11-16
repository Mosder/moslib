#include "heap_tests.h"
#include "moslib/ds/heap.h"

#include <string.h>

TEST(adding) {
    int *min_heap = heap_new(HEAP_MIN);
    int *max_heap = heap_new(HEAP_MIN);

    heap_add(min_heap, 2);
    heap_add(min_heap, 1);
    heap_add(min_heap, 3);
    test_assert(heap_size(min_heap) == 3, "wrong size after adding elements");
    int *top = heap_top(min_heap);
    test_assert(*top == 1, "wrong top element after adding to min_heap");
    heap_free(min_heap);

    heap_add(max_heap, 2);
    heap_add(max_heap, 1);
    heap_add(max_heap, 3);
    test_assert(heap_size(max_heap) == 3, "wrong size after adding elements");
    top = heap_top(max_heap);
    test_assert(*top == 1, "wrong top element after adding to min_heap");

    for (int i = 0; i < 100; i++) {
        heap_add(max_heap, i);
    }
    test_assert(heap_size(max_heap) == 103, "wrong size after adding more elments than INIT_CAP");

    heap_free(max_heap);
}

int heap_tests_cmp_fn(const void *el1, const void *el2) {
    char *e1 = *(char **)el1;
    char *e2 = *(char **)el2;
    return strcmp(e1, e2);
}

TEST(getting) {
    char **heap = heap_new(HEAP_MIN, .cmp = heap_tests_cmp_fn);
    test_assert(heap_top(heap) == NULL, "heap_top for empty heap isn't NULL");

    heap_add(heap, "b");

    heap_add(heap, "c");
    test_assert(strcmp(*heap_top(heap), "b") == 0, "heap_top returned wrong element");
    heap_add(heap, "d");
    test_assert(strcmp(*heap_top(heap), "b") == 0, "heap_top returned wrong element");
    heap_add(heap, "e");
    test_assert(strcmp(*heap_top(heap), "b") == 0, "heap_top returned wrong element");

    heap_add(heap, "a");
    test_assert(strcmp(*heap_top(heap), "a") == 0, "heap_top returned wrong element");

    heap_free(heap);
}

TEST(popping) {
    size_t *heap = NULL;

    size_t top = heap_pop(heap);
    test_assert(top == 0, "heap_pop of uninitialized heap didn't return 0");

    heap_add(heap, 3);
    heap_add(heap, 2);
    heap_add(heap, 1);

    top = heap_pop(heap);
    test_assert(top == 1, "heap_pop returned wrong value");
    test_assert(heap_size(heap) == 2, "size is wrong after heap_pop");
    top = heap_pop(heap);
    test_assert(top == 2, "heap_pop returned wrong value");
    test_assert(heap_size(heap) == 1, "size is wrong after heap_pop");
    top = heap_pop(heap);
    test_assert(top == 3, "heap_pop returned wrong value");
    test_assert(heap_size(heap) == 0, "size is wrong after heap_pop");

    top = heap_pop(heap);
    test_assert(top == 0, "heap_pop of empty heap didn't return 0");

    heap_free(heap);
}

TEST(deleting) {
    float *heap = NULL;
    int del = heap_del_top(heap);
    test_assert(del == 0, "heap_del didn't return 0 for uninitialized heap");

    heap_add(heap, 3.0);
    heap_add(heap, 1.0);

    del = heap_del_top(heap);
    float *top = heap_top(heap);
    test_assert(del == 1 && *top == 1.0, "heap_del didn't delete the top element");
    test_assert(heap_size(heap) == 1, "size is wrong after heap_del_top");

    del = heap_del_top(heap);
    test_assert(del == 1, "heap_del didn't delete the top element");
    test_assert(heap_size(heap) == 0, "size is wrong after heap_del_top");

    del = heap_del_top(heap);
    test_assert(del == 0, "heap_del deleted the top element from empty heap");

    heap_free(heap);
}

void ds_heap_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/ds/heap.h");
    add_test(group, adding);
    add_test(group, getting);
    add_test(group, popping);
    add_test(group, deleting);
}
