#include <moslib/tester.h>

#include "ds/array_tests.h"
#include "ds/hashmap_tests.h"
#include "safe_tests.h"
#include "string_tests.h"

int main(void) {
    Tester *tester = new_tester();

    safe_tests(tester);
    ds_array_tests(tester);
    string_tests(tester);
    ds_hashmap_tests(tester);

    return !run_tests(tester);
}
