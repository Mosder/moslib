#define MOS_FORCE_PREFIXES
#define _GNU_SOURCE
#include "moslib/string.h"

#include <ctype.h>
#include <string.h>

#include "moslib/ds/array.h"

MosStringSlice mos_ss_from_str(const char *str) {
    return mos_ss_from_vars(str, strlen(str));
}

MosStringSlice mos_ss_copy(MosStringSlice ss) {
    return mos_ss_from_vars(ss.data, ss.len);
}

MosStringSlice mos_ss_from_arr(char *arr) {
    return mos_ss_from_vars(arr, mos_arr_len(arr));
}

MosStringSlice mos_ss_from_vars(const char *str, size_t len) {
    return (MosStringSlice){.data = str, .len = len};
}

void mos_ss_print(MosStringSlice ss) {
    printf(MOS_SS_FMT, MOS_SS_ARGS(ss));
}

int mos_ss_eq(MosStringSlice ss1, MosStringSlice ss2) {
    if (ss1.len != ss2.len)
        return 0;

    return memcmp(ss1.data, ss2.data, ss1.len) == 0;
}

int mos_ss_eq_str(MosStringSlice ss, const char *str) {
    return mos_ss_eq(ss, mos_ss_from_str(str));
}

MosStringSlice mos_ss_trim(MosStringSlice ss) {
    return mos_ss_trim_right(mos_ss_trim_left(ss));
}

MosStringSlice mos_ss_trim_left(MosStringSlice ss) {
    size_t i = 0;
    while (i < ss.len && isspace(ss.data[i])) {
        i++;
    }

    ss.data += i;
    ss.len -= i;
    return ss;
}

MosStringSlice mos_ss_trim_right(MosStringSlice ss) {
    size_t i = ss.len;
    while (i-- > 0 && isspace(ss.data[i]))
        ;

    ss.len = i + 1;
    return ss;
}

int mos_ss_starts_with(MosStringSlice ss, MosStringSlice prefix) {
    if (prefix.len > ss.len)
        return 0;

    return memcmp(ss.data, prefix.data, prefix.len) == 0;
}

int mos_ss_ends_with(MosStringSlice ss, MosStringSlice suffix) {
    if (suffix.len > ss.len)
        return 0;

    return memcmp(ss.data + ss.len - suffix.len, suffix.data, suffix.len) == 0;
}

int mos_ss_starts_with_str(MosStringSlice ss, const char *prefix) {
    return mos_ss_starts_with(ss, mos_ss_from_str(prefix));
}

int mos_ss_ends_with_str(MosStringSlice ss, const char *suffix) {
    return mos_ss_ends_with(ss, mos_ss_from_str(suffix));
}

size_t mos_ss_del_left(MosStringSlice *ss, size_t n) {
    if (n > ss->len)
        n = ss->len;

    ss->data += n;
    ss->len -= n;
    return n;
}

size_t mos_ss_del_right(MosStringSlice *ss, size_t n) {
    if (n > ss->len)
        n = ss->len;

    ss->len -= n;
    return n;
}

int mos_ss_del_prefix(MosStringSlice *ss, MosStringSlice prefix) {
    if (!mos_ss_starts_with(*ss, prefix))
        return 0;

    mos_ss_del_left(ss, prefix.len);
    return 1;
}

int mos_ss_del_suffix(MosStringSlice *ss, MosStringSlice suffix) {
    if (!mos_ss_ends_with(*ss, suffix))
        return 0;

    mos_ss_del_right(ss, suffix.len);
    return 1;
}

int mos_ss_del_prefix_str(MosStringSlice *ss, char *prefix) {
    return mos_ss_del_prefix(ss, mos_ss_from_str(prefix));
}

int mos_ss_del_suffix_str(MosStringSlice *ss, char *suffix) {
    return mos_ss_del_suffix(ss, mos_ss_from_str(suffix));
}

MosStringSlice mos_ss_split(MosStringSlice *ss, char delim) {
    size_t i = 0;
    while (i < ss->len && ss->data[i] != delim)
        i++;

    if (i >= ss->len) {
        MosStringSlice res = mos_ss_copy(*ss);
        ss->len = 0;
        return res;
    }

    MosStringSlice res = mos_ss_from_vars(ss->data, i);
    ss->data += i + 1;
    ss->len -= i + 1;
    return res;
}

MosStringSlice mos_ss_split_str(MosStringSlice *ss, const char *delim) {
    return mos_ss_split_ss(ss, mos_ss_from_str(delim));
}

MosStringSlice mos_ss_split_ss(MosStringSlice *ss, MosStringSlice delim) {
    if (delim.len == 0)
        return MOS_SS_LIT("");

    const char *found = memmem(ss->data, ss->len, delim.data, delim.len);
    if (!found) {
        MosStringSlice res = mos_ss_copy(*ss);
        ss->len = 0;
        return res;
    }

    size_t i = found - ss->data;
    MosStringSlice res = mos_ss_from_vars(ss->data, i);
    ss->data += i + delim.len;
    ss->len -= i + delim.len;
    return res;
}

MosStringSlice mos_ss_split_left(MosStringSlice *ss, size_t n) {
    if (n > ss->len)
        n = ss->len;

    MosStringSlice left = mos_ss_from_vars(ss->data, n);
    ss->data += n;
    ss->len -= n;
    return left;
}

MosStringSlice mos_ss_split_right(MosStringSlice *ss, size_t n) {
    if (n > ss->len)
        n = ss->len;

    MosStringSlice right = mos_ss_from_vars(ss->data + ss->len - n, n);
    ss->len -= n;
    return right;
}

MosStringSlice mos_ss_find(MosStringSlice ss, MosStringSlice target) {
    if (target.len == 0)
        return ss;

    const char *found = memmem(ss.data, ss.len, target.data, target.len);
    if (!found)
        return MOS_SS_LIT("");

    return mos_ss_from_vars(found, ss.len - (found - ss.data));
}

MosStringSlice mos_ss_find_str(MosStringSlice ss, const char *target) {
    return mos_ss_find(ss, mos_ss_from_str(target));
}

size_t mos_ss_utf8_len(MosStringSlice ss) {
    size_t utf8_len = ss.len;
    for (size_t i = 0; i < ss.len; i++) {
        char c = ss.data[i];
        if ((c & 0xC0) != 0xC0)
            continue;

        if ((c & 0xE0) == 0xC0)
            utf8_len -= 1;
        else if ((c & 0xF0) == 0xE0)
            utf8_len -= 2;
        else if ((c & 0xF8) == 0xF0)
            utf8_len -= 3;
    }
    return utf8_len;
}
