#include <moslib/tester.h>

#include "ds/array_tests.h"
#include "ds/hashmap_tests.h"
#include "mem/alloc_tests.h"
#include "string_tests.h"

int main(void) {
    Tester *tester = new_tester();

    mem_alloc_tests(tester);
    ds_array_tests(tester);
    string_tests(tester);
    ds_hashmap_tests(tester);

    run_tests(tester);
    free_tester(tester);
    return 0;
}
