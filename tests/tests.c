#include <moslib/tester.h>

#include "mem/alloc_tests.h"

int main(void) {
    Tester *tester = new_tester();

    mem_alloc_tests(tester);

    run_tests(tester);
    free_tester(tester);
    return 0;
}
