#include "compat.h"

#define MOS_FORCE_PREFIXES
#include "moslib/build.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "moslib/ds/array.h"
#include "moslib/safe.h"
#include "moslib/string.h"

#ifndef _WIN32
#define mos_pclose pclose
#define mos_struct_stat struct stat
#define mos_stat stat
#else
#define mos_pclose _pclose
#define mos_struct_stat struct _stat
#define mos_stat _stat
#endif // _WIN32

typedef void (*target_t)(void);

static char *build_cmd(int quiet, va_list parts) {
    char *cmd = NULL;
    const char *arg = NULL;
    while ((arg = va_arg(parts, char *))) {
        mos_arr_append_n(cmd, arg, strlen(arg));
        mos_arr_append(cmd, ' ');
    }
    cmd[mos_arr_len(cmd) - 1] = '\0';
    if (!quiet)
        printf("%s\n", cmd);
    return cmd;
}

void mos_cmd_run_fn(int quiet, ...) {
    va_list args;
    va_start(args, quiet);
    char *cmd = build_cmd(quiet, args);
    va_end(args);

    system(cmd);

    mos_arr_free(cmd);
}

char *mos_cmd_run_out_fn(int quiet, ...) {
    va_list args;
    va_start(args, quiet);
    char *cmd = build_cmd(quiet, args);
    va_end(args);

    char *out = NULL;
    FILE *stream = mos_safe_popen(cmd, "r");
    char buf[512];
    size_t bytes;
    while ((bytes = fread(buf, sizeof(char), sizeof(buf), stream)))
        mos_arr_append_n(out, buf, bytes);

    for (size_t i = 0; i < mos_arr_len(out); i++) {
        if (out[i] == '\n')
            out[i] = ' ';
    }
    mos_arr_append(out, '\0');

    mos_pclose(stream);
    mos_arr_free(cmd);
    return out;
}

void mos_build_target_fn(int argc, char *argv[], const char *names, ...) {
    va_list targets;
    va_start(targets, names);
    target_t target = va_arg(targets, target_t);
    if (argc < 2) {
        va_end(targets);
        target();
        return;
    }

    MosStringSlice t = mos_ss_from_str(argv[1]);
    MosStringSlice ns = mos_ss_from_str(names);
    do {
        MosStringSlice n = mos_ss_trim(mos_ss_split(&ns, ','));
        if (mos_ss_eq(n, t)) {
            va_end(targets);
            target();
            return;
        }
    } while ((target = va_arg(targets, target_t)));

    va_end(targets);
    fprintf(stderr, "\nTarget %s not found\n", argv[1]);
}

static int rebuild_needed(const char *src, const char *bin) {
    mos_struct_stat s, b;
    if (mos_stat(src, &s) || mos_stat(bin, &b)) {
        printf("Couldn't determine if rebuild is needed\n\n");
        return 0;
    }
    if (s.st_mtime > b.st_mtime)
        return 1;
    return 0;
}

void mos_build_rebuild_fn(int argc, char *argv[], const char *file, ...) {
    if (!rebuild_needed(file, argv[0]))
        return;

    va_list args;
    va_start(args, file);
    char *cmd = build_cmd(1, args);
    va_end(args);
    cmd[mos_arr_len(cmd) - 1] = ' ';

    mos_arr_append_n(cmd, file, strlen(file));
    mos_arr_append_n(cmd, " -o ", 4);
    mos_arr_append_n(cmd, argv[0], strlen(argv[0]));
    mos_arr_append(cmd, '\0');
    system(cmd);
    mos_arr_free(cmd);
    printf("Rebuilt.\n\n");

    char *new_build = NULL;
    for (int i = 0; i < argc; i++) {
        mos_arr_append_n(new_build, argv[i], strlen(argv[i]));
        mos_arr_append(new_build, ' ');
    }
    system(new_build);
    exit(0);
}
