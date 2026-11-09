// moslib/build.h
// Simple build system in C

// ======================================= QUICK USAGE GUIDE =======================================
// -------------------------------------------------------------------------------------------------
//
// Run commands using cmd_run:
//
//      cmd_run("rm *.o");
//      cmd_run_q(CC, CFLAGS, SRCS, "-o main");
//
// You can run command and capture its output using cmd_run_out:
//
//      char *srcs = cmd_run_out("find src | grep \\\\.c$");
//      char *sudo = cmd_run_out_q("test -w", TARGET_DIR, "|| echo sudo");
//
// The versions suffixed with _q will not print the command during build time.
//
// -------------------------------------------------------------------------------------------------
//
// Define build targets using TARGET macro:
//
//      TARGET(compile) {
//          cmd_run("cc main.c -o main");
//      }
//
//      TARGET(clean) {
//          cmd_run("rm *.o");
//      }
//
// -------------------------------------------------------------------------------------------------
//
// To use defined targets, use build_target in main:
//
//      int main(int argc, char *argv) {
//          build_target(compile, clean);
//      }
//
// When run with no arguments it will use the first target.
// It expects argc and argv to be defined with these exact names.
//
// -------------------------------------------------------------------------------------------------
//
// You can make the build binary rebuild itself automatically.
// It will detect if the build.c file was changed and recompile if it did.
// To do so, use build_rebuild at the top of main:
//
//      int main(int argc, char *argv) {
//          build_rebuild(CC, "-lmoslib");
//          build_target(compile, clean);
//      }
//
// It accepts arguments in the same way as cmd_run and should receive the command initially used for
// compilation with exceptions of the input and output.
// I.e. if you, for example, compile with:
//      CC CFLAGS -lmoslib build.c -o build
// It should be used something like this:
//      build_rebuild(CC, CFLAGS, "-lmoslib");
//
// If the absolute path of build.c changed - it will not be able to detect any changes and the build
// binary will have to be recompiled by hand again.
// It expects argc and argv to be defined with these exact names.
//
// -------------------------------------------------------------------------------------------------
// =================================== END OF QUICK USAGE GUIDE ====================================

#include <stddef.h>

#ifndef MOSLIB_BUILD_H
#define MOSLIB_BUILD_H

#ifndef MOS_FORCE_PREFIXES

#define TARGET MOS_TARGET
#define cmd_run mos_cmd_run
#define cmd_run_q mos_cmd_run_q
#define cmd_run_out mos_cmd_run_out
#define cmd_run_out_q mos_cmd_run_out_q
#define build_target mos_build_target
#define build_rebuild mos_build_rebuild

#endif // MOS_FORCE_PREFIXES

#define MOS_TARGET(name) static void name(void)

// Run a command
//
// Arguments:
//   ...
//     strings representing the command to be run
//     every argument is automatically space separated
#define mos_cmd_run(...) mos_cmd_run_fn(0, __VA_ARGS__, (char *)NULL)

// Run a command without printing it during build
//
// Arguments:
//   ...
//     strings representing the command to be run
//     every argument is automatically space separated
#define mos_cmd_run_q(...) mos_cmd_run_fn(1, __VA_ARGS__, (char *)NULL)

// Run a command and capture its output
//
// Arguments:
//   ...
//     strings representing the command to be run
//     every argument is automatically space separated
//
// Returns:
//   output of the command
#define mos_cmd_run_out(...) mos_cmd_run_out_fn(0, __VA_ARGS__, (char *)NULL)

// Run a command without printing it during build and capture its output
//
// Arguments:
//   ...
//     strings representing the command to be run
//     every argument is automatically space separated
//
// Returns:
//   output of the command
#define mos_cmd_run_out_q(...) mos_cmd_run_out_fn(1, __VA_ARGS__, (char *)NULL)

// Target the given targets
// When no argument is given - targets the first target
//
// Arguments:
//   ...
//     targets which can be targetted
#define mos_build_target(...)                                                                      \
    mos_build_target_fn(argc, argv, #__VA_ARGS__, __VA_ARGS__, (char *)NULL)

// Allow automatic recompiling of the build binary
//
// Arguments:
//   ...
//     command used for compilation of the build binary,
//     with the exception of input file and output file
#define mos_build_rebuild(...) mos_build_rebuild_fn(argc, argv, __FILE__, __VA_ARGS__, (char *)NULL)

// Function prototypes for macros
extern void mos_cmd_run_fn(int quiet, ...);
extern char *mos_cmd_run_out_fn(int quiet, ...);
extern void mos_build_target_fn(int argc, char *argv[], const char *names, ...);
extern void mos_build_rebuild_fn(int argc, char *argv[], const char *file, ...);

#endif // MOSLIB_BUILD_H
