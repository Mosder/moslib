#define MOS_FORCE_PREFIXES
#include "moslib/string.h"

MosStringSlice mos_ss_from_str(const char *str) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_copy(MosStringSlice ss) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_from_arr(char *arr) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_from_vars(const char *str, size_t len) {
    return (MosStringSlice){0};
}

void mos_ss_print(MosStringSlice ss) {
}

int mos_ss_eq(MosStringSlice ss1, MosStringSlice ss2) {
    return 0;
}

int mos_ss_eq_str(MosStringSlice ss, const char *str) {
    return 0;
}

MosStringSlice mos_ss_trim(MosStringSlice ss) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_trim_left(MosStringSlice ss) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_trim_right(MosStringSlice ss) {
    return (MosStringSlice){0};
}

int mos_ss_starts_with(MosStringSlice ss, MosStringSlice prefix) {
    return 0;
}

int mos_ss_ends_with(MosStringSlice ss, MosStringSlice suffix) {
    return 0;
}

int mos_ss_starts_with_str(MosStringSlice ss, const char *prefix) {
    return 0;
}

int mos_ss_ends_with_str(MosStringSlice ss, const char *suffix) {
    return 0;
}

size_t mos_ss_del_left(MosStringSlice *ss, size_t n) {
    return 0;
}

size_t mos_ss_del_right(MosStringSlice *ss, size_t n) {
    return 0;
}

int mos_ss_del_prefix(MosStringSlice *ss, MosStringSlice prefix) {
    return 0;
}

int mos_ss_del_suffix(MosStringSlice *ss, MosStringSlice suffix) {
    return 0;
}

int mos_ss_del_prefix_str(MosStringSlice *ss, char *prefix) {
    return 0;
}

int mos_ss_del_suffix_str(MosStringSlice *ss, char *suffix) {
    return 0;
}

MosStringSlice mos_ss_split(MosStringSlice *ss, char delim) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_split_str(MosStringSlice *ss, const char *delim) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_split_ss(MosStringSlice *ss, MosStringSlice delim) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_split_left(MosStringSlice *ss, size_t n) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_split_right(MosStringSlice *ss, size_t n) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_find(MosStringSlice ss, MosStringSlice target) {
    return (MosStringSlice){0};
}

MosStringSlice mos_ss_find_str(MosStringSlice ss, const char *target) {
    return (MosStringSlice){0};
}

size_t mos_ss_utf8_len(MosStringSlice ss) {
    return 0;
}
