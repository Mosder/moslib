#include ".build.h"

// TODO: LIB_SUDO, INC_SUDO, SRC_SRCS, TESTS_SRCS, probe_fun + probed

#ifndef CC
#define CC "cc"
#endif // CC

#ifndef TARGET_PREFIX
#define TARGET_PREFIX "/usr/local"
#endif // TARGET_PREFIX

#define AR "ar rcs"

#define LIB_DIR TARGET_PREFIX "/lib"
#define INC_DIR TARGET_PREFIX "/include"

#define COMMON_CFLAGS "-std=c99 -Wall -Wextra -Wpedantic -c -I./compat"
#define SRC_CFLAGS "-fPIC -I./include -O2"
#define TESTS_CFLAGS "-I" INC_DIR " -Og -ggdb"
#define TESTS_LDFLAGS "-L" LIB_DIR " -Wl,-rpath," LIB_DIR " -lmoslib"

void compile(void) {
    cmd_run(CC, COMMON_FLAGS, SRC_FLAGS, SRC_SRCS);
}

void headers(void) {
    cmd_run("cp -r", "include/*", INC_DIR);
}

void shrd(void) {
    cmd_run(CC, "-shared *.o -o", LIB_DIR "/libmoslib.so");
    cmd_run("strip -x", LIB_DIR "/libmoslib.so");
}

void stat(void) {
    cmd_run(AR, LIB_DIR "/libmoslib.a", "*.o");
    cmd_run("strip -S", LIB_DIR "/libmoslib.a");
}

TARGET(both) {
    clean();
    compile();
    headers();
    shrd();
    stat();
    clean();
}

TARGET(shared) {
    clean();
    compile();
    headers();
    shrd();
    clean();
}

TARGET(static) {
    clean();
    compile();
    headers();
    stat();
    clean();
}

TARGET(test) {
    clean();
    cmd_run(CC, TESTS_CFLAGS, TESTS_SRCS);
    cmd_run(CC, TESTS_LDFLAGS, "*.o -o run_tests");
    cmd_run("./run_tests");
    clean();
}

TARGET(clean) {
    cmd_run("rm *.o run_tests 2>/dev/null");
    cmd_run("rm -rf *.dSYM 2>/dev/null");
}

int main(int argc, char *argv[]) {
    build_rebuild();
    build_target(both, shared, static, test, clean);
}
