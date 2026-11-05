#include ".build.h"

#include "include/moslib.h"

// Include implementation files, so building this library doesn't depend on it being already built
#include "src/build.c"
#include "src/ds/array.c"
#include "src/safe.c"

#ifndef CC
#define CC "cc"
#endif // CC

#ifndef TARGET_PREFIX
#define TARGET_PREFIX "/usr/local"
#endif // TARGET_PREFIX

#define AR "ar rcs"

#define LIB_DIR TARGET_PREFIX "/lib"
#define INC_DIR TARGET_PREFIX "/include"

#define CFLAGS "-std=c99 -Wall -Wextra -Wpedantic -c -I./compat"
#define SRC_CFLAGS "-fPIC -I./include -O2"
#define TESTS_CFLAGS "-I" INC_DIR " -Og -ggdb"
#define TESTS_LDFLAGS "-L" LIB_DIR " -Wl,-rpath," LIB_DIR " -lmoslib"

static char *src_srcs = NULL;
static char *tests_srcs = NULL;
static char *lib_sudo = NULL;
static char *inc_sudo = NULL;
static char *probed = NULL;

static void probe_fun(char *header, char *fun) {
    char *p = cmd_run_out(
        "printf '#include \"compat.h\"\n#include",
        header,
        "\nint main(void) {",
        fun,
        "; return 0; }' |",
        CC,
        CFLAGS,
        "-x c - -o /dev/null 2>/dev/null || echo -DNO_$(echo",
        fun,
        "| tr a-z A-Z))"
    );
    arr_append_n(probed, p, strlen(p));
}

static void compile(void) {
    cmd_run(CC, CFLAGS, SRC_CFLAGS, src_srcs);
}

static void headers(void) {
    cmd_run(inc_sudo, "cp -r", "include/*", INC_DIR);
}

static void shared_(void) {
    cmd_run(lib_sudo, CC, "-shared *.o -o", LIB_DIR "/libmoslib.so");
    cmd_run(lib_sudo, "strip -x", LIB_DIR "/libmoslib.so");
}

static void stat_(void) {
    cmd_run(lib_sudo, AR, LIB_DIR "/libmoslib.a", "*.o");
    cmd_run(lib_sudo, "strip -S", LIB_DIR "/libmoslib.a");
}

TARGET(clean) {
    cmd_run("rm *.o run_tests 2>/dev/null");
    cmd_run("rm -rf *.dSYM 2>/dev/null");
}

TARGET(both) {
    clean();
    compile();
    headers();
    shared_();
    stat_();
    clean();
}

TARGET(shared) {
    clean();
    compile();
    headers();
    shared_();
    clean();
}

TARGET(stat) {
    clean();
    compile();
    headers();
    stat_();
    clean();
}

TARGET(test) {
    clean();
    cmd_run(CC, CFLAGS, TESTS_CFLAGS, tests_srcs);
    cmd_run(CC, TESTS_LDFLAGS, "*.o -o run_tests");
    cmd_run("./run_tests");
    clean();
}

int main(int argc, char *argv[]) {
    build_rebuild(CC);

    src_srcs = cmd_run_out("find src compat | grep \\.c$");
    tests_srcs = cmd_run_out("find src compat | grep \\.c$");
    lib_sudo = cmd_run_out("test -w", LIB_DIR, "|| echo sudo");
    inc_sudo = cmd_run_out("test -w", INC_DIR, "|| echo sudo");

    probe_fun("<string.h>", "memmem");
    probe_fun("<string.h>", "strdup");

    build_target(both, shared, stat, test, clean);
}
