#include ".build.h"

#include "include/moslib.h"

// Include implementation files, so building this library doesn't depend on it being already built
#include "src/build.c"
#include "src/ds/array.c"
#include "src/safe.c"
#include "src/string.c"

#ifndef CC
#define CC "cc"
#endif // CC

#ifndef TARGET_PREFIX
#define TARGET_PREFIX "/usr/local"
#endif // TARGET_PREFIX

#define AR "ar rcs"

#define LIB_DIR TARGET_PREFIX "/lib"
#define INC_DIR TARGET_PREFIX "/include"

#define CFLAGS "-std=c99 -Wall -Wextra -Wpedantic -c -I./compat -I./include"
#define SRC_CFLAGS "-fPIC -O2"
#define TESTS_CFLAGS "-Og -ggdb"

#if defined(__APPLE__)
#define SHLIB_FILE LIB_DIR "/libmoslib.dylib"
#define SHLIB_FLAGS "-dynamiclib"
#elif defined(_WIN32)
#define SHLIB_FILE TARGET_PREFIX "/bin/moslib.dll"
#define SHLIB_FLAGS "-shared -Wl,--out-implib," LIB_DIR "/libmoslib.dll.a"
#else
#define SHLIB_FILE LIB_DIR "/libmoslib.so"
#define SHLIB_FLAGS "-shared"
#endif

static char *srcs = NULL;
static char *tests_srcs = NULL;
static char *lib_sudo = NULL;
static char *inc_sudo = NULL;
static char *probed = NULL;

static void probe_fun(char *header, char *fun) {
    char *p = cmd_run_out_q(
        "printf '#include \"compat.h\"\\n#include",
        header,
        "\\nint main(void) {",
        fun,
        "; return 0; }' |",
        CC,
        CFLAGS,
        "-x c - -o /dev/null 2>/dev/null || echo -DNO_$(echo",
        fun,
        "| tr a-z A-Z)"
    );
    arr_append_n(probed, p, strlen(p));
}

static void compile(void) {
    cmd_run(CC, CFLAGS, SRC_CFLAGS, probed, srcs);
}

static void headers(void) {
    cmd_run(inc_sudo, "cp -r", "include/*", INC_DIR);
}

static void sh(void) {
    cmd_run(lib_sudo, CC, SHLIB_FLAGS, "*.o -o", SHLIB_FILE);
    cmd_run(lib_sudo, "strip -x", SHLIB_FILE);
}

static void st(void) {
    cmd_run(lib_sudo, AR, LIB_DIR "/libmoslib.a", "*.o");
    cmd_run(lib_sudo, "strip -S", LIB_DIR "/libmoslib.a");
}

TARGET(clean) {
    cmd_run_q("rm *.o run_tests 2>/dev/null");
    cmd_run_q("rm -rf *.dSYM 2>/dev/null");
}

TARGET(both) {
    clean();
    compile();
    headers();
    sh();
    st();
    clean();
}

TARGET(so) {
    clean();
    compile();
    headers();
    sh();
    clean();
}

TARGET(a) {
    clean();
    compile();
    headers();
    st();
    clean();
}

TARGET(test) {
    clean();
    cmd_run(CC, CFLAGS, TESTS_CFLAGS, probed, srcs, tests_srcs);
    cmd_run(CC, "*.o -o run_tests");
    cmd_run("./run_tests");
    clean();

    // Test that it works on Linux + glibc
    cmd_run_q(
        "docker run --rm -v .:/src:ro gcc:latest sh -c \"",
        "cp -r /src /moslib && cd /moslib",
        "&& cc",
        CFLAGS,
        TESTS_CFLAGS,
        srcs,
        tests_srcs,
        "&& cc *.o -o run_tests && ./run_tests > /dev/null",
        "|| echo '\\n--- FAILED ON GLIBC LINUX ---'\""
    );
}

int main(int argc, char *argv[]) {
    build_rebuild(CC, "-Icompat", "-Iinclude");

    srcs = cmd_run_out_q("find src compat | grep \\\\.c$");
    tests_srcs = cmd_run_out_q("find tests | grep \\\\.c$");
    lib_sudo = cmd_run_out_q("test -w", LIB_DIR, "|| echo sudo");
    inc_sudo = cmd_run_out_q("test -w", INC_DIR, "|| echo sudo");

    probe_fun("<string.h>", "memmem");
    probe_fun("<string.h>", "strdup");
    arr_append(probed, '\0');

    build_target(both, so, a, test, clean);
}
