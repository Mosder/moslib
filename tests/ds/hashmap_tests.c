#define _GNU_SOURCE
#define MOS_HM_LOAD_FACTOR 1
#include "hashmap_tests.h"
#include "moslib/ds/hashmap.h"

#include <stdlib.h>
#include <string.h>

#include "moslib/ds/array.h"
#include "moslib/string.h"

#define INIT_CAP 16

TEST(put_get) {
    HM_DEF(E, float, int);
    E *hm = NULL;

    hm_put(hm, 0.0f, 69);
    hm_put(hm, 1.0f, 420);
    test_assert(hm_get(hm, 0.0f) == 69, "hm_get got value not from hm_put");
    test_assert(hm_get(hm, 1.0f) == 420, "hm_get got value not from hm_put");

    hm_put(hm, 0.0f, 2137);
    test_assert(hm_get(hm, 0.0f) == 2137, "hm_put didn't update value properly");
    test_assert(hm_size(hm) == 2, "hm_size returned wrong entry count");

    for (int i = 2; i < 2 * INIT_CAP + 2; i++) {
        hm_put(hm, (float)i, i);
    }
    int flag = 1;
    for (int i = 2; i < 2 * INIT_CAP + 2; i++) {
        if (hm_get(hm, (float)i) != i)
            flag = 0;
    }
    test_assert(flag, "hm_put / hm_get failed when expansion was needed");
    test_assert(hm_size(hm) == 2 * INIT_CAP + 2, "hm_size returned wrong entry count");

    hm_free(hm);
}

TEST(put_e_get_e) {
    HM_DEF(E, float, int);
    E *hm = NULL;

    hm_put_e(hm, (E){0.0f, 69});
    hm_put_e(hm, (E){1.0f, 420});
    E e = {0.0f, 69};
    test_assert(memcmp(&e, hm_get_e(hm, 0.0f), sizeof(E)) == 0, "hm_get_e got entry not from hm_put_e");
    e = (E){1.0f, 420};
    test_assert(memcmp(&e, hm_get_e(hm, 1.0f), sizeof(E)) == 0, "hm_get_e got entry not from hm_put_e");

    e = (E){1.0f, 2137};
    hm_put_e(hm, e);
    test_assert(memcmp(&e, hm_get_e(hm, 1.0f), sizeof(E)) == 0, "hm_put_e didn't update entry properly");

    hm_free(hm);
}

TEST(deleting) {
    HM_DEF(E, long double, float);
    E *hm = NULL;

    hm_put(hm, 1.0, 1.0);
    hm_put(hm, 2.0, 2.0);

    int del = hm_del(hm, 1.0);
    test_assert(hm_get_e(hm, 1.0) == NULL, "hm_del didn't delete entry it supposed to");
    test_assert(del == 1, "hm_del didn't return 1 when it deleted");
    test_assert(hm_size(hm) == 1, "hm_size after successful hm_del is incorrect");

    del = hm_del(hm, 1.0);
    test_assert(del == 0, "hm_del didn't return 0 when it didn't delete");
    test_assert(hm_get(hm, 2.0) == 2.0, "hm_del deleted wrong entry");
    test_assert(hm_size(hm) == 1, "hm_size after failed hm_del is incorrect");

    for (int i = 3; i < 2 * INIT_CAP + 3; i++) {
        hm_put(hm, (long double)i, (float)i);
    }
    test_assert(hm_get_e(hm, 1.0) == NULL, "expansion revived entry");
    test_assert(hm_size(hm) == 2 * INIT_CAP + 1, "hm_size after expansion is incorrect");

    hm_free(hm);
}

TEST(str_key) {
    HM_DEF(E, char *, int);
    E *hm = hm_new(.key = STR);

    char *str = "key";
    char *str2 = strdup(str);
    hm_put(hm, str, 69);
    hm_put(hm, "hey", 420);
    test_assert(hm_get(hm, str2) == 69, "hm_get got value not from hm_put");
    test_assert(hm_get(hm, "hey") == 420, "hm_get got value not from hm_put");

    hm_put(hm, str2, 2137);
    test_assert(hm_get(hm, str) == 2137, "hm_put didn't update value properly");

    free(str2);
    hm_free(hm);
}

TEST(ss_key) {
    HM_DEF(E, StringSlice, int);
    E *hm = hm_new(.key = SS);

    char *str = "key";
    char *str2 = strdup(str);
    StringSlice ss = ss_from_str(str);
    StringSlice ss2 = ss_from_str(str2);
    hm_put(hm, ss, 69);
    hm_put(hm, SS_LIT("hey"), 420);
    test_assert(hm_get(hm, ss2) == 69, "hm_get got value not from hm_put");
    test_assert(hm_get(hm, SS_LIT("hey")) == 420, "hm_get got value not from hm_put");

    hm_put(hm, ss2, 2137);
    test_assert(hm_get(hm, ss) == 2137, "hm_put didn't update value properly");

    free(str2);
    hm_free(hm);
}

TEST(looping) {
    HM_DEF(E, int, int);
    E *hm = NULL;

    hm_put(hm, 1, 10);
    hm_put(hm, 2, 20);
    hm_put(hm, 3, 30);

    E *allowed = NULL;
    E items[] = {{1, 10}, {2, 20}, {3, 30}};
    arr_append_n(allowed, items, 3);

    test_assert(memcmp(hm_first(hm), hm_next(hm, NULL), sizeof(E)) == 0, "hm_next(hm, NULL) != hm_first(hm)");
    E *e = NULL;
    while ((e = hm_next(hm, e))) {
        int flag = 0;
        for (size_t i = 0; i < arr_len(allowed); i++) {
            if (memcmp(e, allowed + i, sizeof(E)) == 0) {
                flag = 1;
                arr_del(allowed, i);
                break;
            }
        }
        test_assert(flag, "hm_next produced an entry that wasn't put in the hashmap");
    }
    test_assert(arr_len(allowed) == 0, "hm_next didn't produce every entry from hashmap when it looped over it");

    arr_free(allowed);
    hm_free(hm);
}

void ds_hashmap_tests(Tester *tester) {
    TestGroup *group = add_test_group(tester, "moslib/ds/hashmap.h");
    add_test(group, put_get);
    add_test(group, put_e_get_e);
    add_test(group, deleting);
    add_test(group, str_key);
    add_test(group, ss_key);
    add_test(group, looping);
}
