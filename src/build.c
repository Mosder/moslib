#define MOS_FORCE_PREFIXES
#include "moslib/build.h"

#include <stdarg.h>
#include <string.h>

#include "moslib/ds/array.h"

static char *build_cmd(const char *cmd_start, va_list cmd_rest) {
    char *cmd = NULL;
    const char *arg = cmd_start;
    do {
        mos_arr_append_n(cmd, arg, strlen(arg));
    } while ((arg = (const char *)va_arg(cmd_rest, char *)));
    mos_arr_append(cmd, '\0');
    return cmd;
}

void mos_cmd_run_fn(const char *cmd_start, ...) {
    va_list args;
    va_start(args, cmd_start);
    char *cmd = build_cmd(cmd_start, args);
    va_end(args);

    // run command

    mos_arr_free(cmd);
}

char *mos_cmd_run_out_fn(const char *cmd_start, ...) {
    va_list args;
    va_start(args, cmd_start);
    char *cmd = build_cmd(cmd_start, args);
    va_end(args);

    char *out = NULL;
    // run command and capture its output

    mos_arr_free(cmd);
    return out;
}

void mos_build_target_fn(int argc, char *argv[], ...) {
    (void)argc;
    (void)argv;
}

void mos_build_rebuild_fn(int argc, char *argv[], const char *file, ...) {
    (void)argc;
    (void)argv;
    (void)file;
}
