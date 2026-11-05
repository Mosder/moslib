#define MOS_FORCE_PREFIXES
#include "moslib/build.h"

void mos_cmd_run_fn(const char *cmd_start, ...) {
    (void)cmd_start;
}

char *mos_cmd_run_out_fn(const char *cmd_start, ...) {
    (void)cmd_start;
    return (void *)0;
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
