// moslib/string.h
// Length aware string slices

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// StringSlice is defined as a struct which contains a pointer to the beginning of a string and its
// length:
//
//      typedef struct {
//          const char *data;
//          size_t len;
//      } StringSlice;
//
// ----------------------------------------------------------------------------------------------------
//
// To get a string slice from a C string you can use:
//
//      char *str = "Hello, World!";
//      StringSlice ss = ss_from_str(str);
//
// If you want to use a string literal to create a slice at compile time, use:
//
//      StringSlice ss = SS_LIT("literal");
//
// You can also use this lib's implementation of dynamic array as a sort of string builder and then
// get a string slice from it:
//
//      char hello[] = "Hello, ";
//      char world[] = "World!";
//      char *sb = NULL;
//      arr_append_n(sb, hello, strlen(hello));
//      arr_append_n(sb, world, strlen(world));
//      StringSlice ss = ss_from_arr(sb);
//
// Be ware that freeing the array will create a dangling pointer in the slice.
//
// You can also just use a wrapper for getting it from 2 variables:
//
//      StringSlice ss = ss_from_vars("[in][out]", 4);
//
// ----------------------------------------------------------------------------------------------------
//
// To print a slice you can either use:
//
//      ss_print(ss);
//
// Which will just print a slice to stdout (without trailing new line).
//
// Or, instead, you can use SS_FMT and SS_ARGS(ss) macros:
//
//      printf("Slice: " SS_FMT "\n", SS_ARGS(ss));
//
// ----------------------------------------------------------------------------------------------------
//
// To check if two slices are equal, use:
//
//      int are_eq = ss_eq(ss1, ss2);
//
// ----------------------------------------------------------------------------------------------------
//
// You can split a slice in two using "split" functions:
//
//      StringSlice ss1 = ss_from_str("Hello, World!");
//      StringSlice ss2 = ss_split(&ss1, ',');                   // ss2 = "Hello", ss3 = " World!"
//      StringSlice ss3 = ss_split_str(&ss2, "ll");              // ss3 = "He",    ss2 = "o"
//      StringSlice ss4 = ss_split_ss(&ss1, ss_from_lit("rld")); // ss4 = " Wo",   ss1 = "!"
//
// They will return a slice containing everything before the delimiter and modify in place the given
// slice to contain everything after the delimiter.
// If the delimiter wasn't found - returned slice will be the same as the given one and the given one
// will be emptied. The same will happen for an empty delimiter in _str / _ss variants.
//
// ----------------------------------------------------------------------------------------------------
//
// You can trim a slice from whitespace using:
//
//      StringSlice trimmed = ss_trim(ss); // same as trim_left + trim_right
//      StringSlice trimmed_l = ss_trim_left(ss);
//      StringSlice trimmed_r = ss_trim_right(ss);
//
// ----------------------------------------------------------------------------------------------------
//
// To check whether a slice starts or ends with a slice / string, you can use:
//
//      int flag1 = ss_starts_with(ss, ss_prefix);
//      int flag2 = ss_ends_with(ss, ss_suffix);
//      int flag3 = ss_starts_with_str(ss, str_prefix);
//      int flag4 = ss_ends_with_str(ss, str_suffix);
//
// ----------------------------------------------------------------------------------------------------
//
// To find within a slice a part which begins with given target, use:
//
//      StringSlice target1 = ss_find(ss, ss_target);
//      StringSlice target2 = ss_find_str(ss, str_target);
//
// The return value is a slice that begins at the beginning of the specified target and ends at the end
// of the original slice.
// If the target was an emptry slice / string - returns the entire slice.
// If the target was not found - returns empty slice.
//
// ----------------------------------------------------------------------------------------------------
//
// If you want to get length of a slice then you can simply use the len field of the struct.
// However, if you want to know the length which respects utf-8 encoding, use:
//
//      size_t utf8_len = ss_utf8_len(ss);
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#include <stddef.h>

#ifndef MOSLIB_STRING_H
#define MOSLIB_STRING_H

#ifndef MOS_FORCE_PREFIXES

#define StringSlice MosStringSlice
#define ss_from_str mos_ss_from_str
#define SS_LIT MOS_SS_LIT
#define ss_from_arr mos_ss_from_arr
#define ss_from_vars mos_ss_from_vars
#define ss_print mos_ss_print
#define SS_FMT MOS_SS_FMT
#define SS_ARGS MOS_SS_ARGS
#define ss_eq mos_ss_eq
#define ss_split mos_ss_split
#define ss_split_str mos_ss_split_str
#define ss_split_ss mos_ss_split_ss
#define ss_trim mos_ss_trim
#define ss_trim_left mos_ss_trim_left
#define ss_trim_right mos_ss_trim_right
#define ss_starts_with mos_ss_starts_with
#define ss_ends_with mos_ss_ends_with
#define ss_starts_with_str mos_ss_starts_with_str
#define ss_ends_with_str mos_ss_ends_with_str
#define ss_find mos_ss_find
#define ss_find_str mos_ss_find_str
#define ss_utf8_len mos_ss_utf8_len

#endif // MOS_FORCE_PREFIXES

typedef struct {
    const char *data;
    size_t len;
} MosStringSlice;

// Create a string slice from a C string
//
// Arguments:
//   str
//     a C string to create a slice from
//
// Returns:
//   a newly created string slice
extern MosStringSlice mos_ss_from_str(const char *str);

// Create a string slice from a string literal at compile time
//
// Arguments:
//   lit
//     a string literal to create a slice from
//
// Returns:
//   a newly created string slice
#define MOS_SS_LIT(lit) (MosStringSlice){.data = (lit), .len = sizeof(lit) - 1}

// Create a string slice from this lib's dynamic array implementation
//
// Arguments:
//   arr
//     dynamic array of characters
//
// Returns:
//   a newly created string slice
extern MosStringSlice mos_ss_from_arr(char *arr);

// Create a string slice from two variables representing it
//
// Arguments:
//   str
//     pointer to the beginning of a slice
//
//   len
//     length of the slice
//
// Returns:
//   a newly created string slice
extern MosStringSlice mos_ss_from_vars(const char *str, size_t len);

// Print a slice to stdout without the trailing new line
//
// Arguments:
//   ss
//     a slice to print
extern void mos_ss_print(MosStringSlice ss);

// Format specifier for printing a slice
#define MOS_SS_FMT "%.*s"

// Argument list to use with the format specifier
#define MOS_SS_ARGS(ss) (int)(ss).len, (ss).data

// Check if two slices are equal
//
// Arguments:
//   ss1
//     first slice to compare
//
//   ss2
//     second slice to compare
//
// Returns:
//   1 if slices are equal, 0 otherwise
extern int mos_ss_eq(MosStringSlice ss1, MosStringSlice ss2);

// Split a slice by a character delimiter
// If the delimiter is not found - treats the end of the slice as the delimiter
//
// Arguments:
//   ss
//     pointer to the slice to split
//     will be modified in place to contain everything after the delimiter
//
//   delim
//     the delimiter to split by
//
// Returns:
//   a slice containing everything before the delimiter
extern MosStringSlice mos_ss_split(MosStringSlice *ss, char delim);

// Split a slice by a C string delimiter
// If the delimiter is not found or is an empty string - treats the end of the slice as the delimiter
//
// Arguments:
//   ss
//     pointer to the slice to split
//     will be modified in place to contain everything after the delimiter
//
//   delim
//     the delimiter to split by
//
// Returns:
//   a slice containing everything before the delimiter
extern MosStringSlice mos_ss_split_str(MosStringSlice *ss, const char *delim);

// Split a slice by a slice delimiter
// If the delimiter is not found or is an empty slice - treats the end of the slice as the delimiter
//
// Arguments:
//   ss
//     pointer to the slice to split
//     will be modified in place to contain everything after the delimiter
//
//   delim
//     the delimiter to split by
//
// Returns:
//   a slice containing everything before the delimiter
extern MosStringSlice mos_ss_split_ss(MosStringSlice *ss, MosStringSlice delim);

// Time a slice of white space from both the left and the right side
//
// Arguments:
//   ss
//     slice to trim
//
// Returns:
//   a trimmed slice
extern MosStringSlice mos_ss_trim(MosStringSlice ss);

// Time a slice of white space from the left side
//
// Arguments:
//   ss
//     slice to trim
//
// Returns:
//   a trimmed slice
extern MosStringSlice mos_ss_trim_left(MosStringSlice ss);

// Time a slice of white space from the right side
//
// Arguments:
//   ss
//     slice to trim
//
// Returns:
//   a trimmed slice
extern MosStringSlice mos_ss_trim_right(MosStringSlice ss);

// Check whether a slice starts with a given slice prefix
//
// Arguments:
//   ss
//     slice to check if it starts with the prefix
//
//   prefix
//     expected prefix of the slice
//
// Returns:
//   1 if the slice starts with the prefix, 0 otherwise
extern int mos_ss_starts_with(MosStringSlice ss, MosStringSlice prefix);

// Check whether a slice ends with a given slice suffix
//
// Arguments:
//   ss
//     slice to check if it ends with the suffix
//
//   suffix
//     expected suffix of the slice
//
// Returns:
//   1 if the slice ends with the suffix, 0 otherwise
extern int mos_ss_ends_with(MosStringSlice ss, MosStringSlice suffix);

// Check whether a slice starts with a given C string prefix
//
// Arguments:
//   ss
//     slice to check if it starts with the prefix
//
//   prefix
//     expected prefix of the slice
//
// Returns:
//   1 if the slice starts with the prefix, 0 otherwise
extern int mos_ss_starts_with_str(MosStringSlice ss, const char *prefix);

// Check whether a slice ends with a given C string suffix
//
// Arguments:
//   ss
//     slice to check if it ends with the suffix
//
//   suffix
//     expected suffix of the slice
//
// Returns:
//   1 if the slice ends with the suffix, 0 otherwise
extern int mos_ss_ends_with_str(MosStringSlice ss, const char *suffix);

// Find the given slice target inside a slice
//
// Arguments:
//   ss
//     slice to find the target in
//
//   target
//     target to find
//
// Returns:
//   a slice starting at target and ending in the same place as the original,
//   the entire slice - if the target was an empty slice,
//   an empty slice - if the target was not found
extern MosStringSlice mos_ss_find(MosStringSlice ss, MosStringSlice target);

// Find the given C string target inside a slice
//
// Arguments:
//   ss
//     slice to find the target in
//
//   target
//     target to find
//
// Returns:
//   a slice starting at target and ending in the same place as the original,
//   the entire slice - if the target was an empty string,
//   an empty slice - if the target was not found
extern MosStringSlice mos_ss_find_str(MosStringSlice ss, const char *target);

// Get the length of the slice with respect to the utf-8 encoding
//
// Arguments:
//   ss
//     slice to get the utf-8 length of
//
// Returns:
//   utf-8 length
extern size_t mos_ss_utf8_len(MosStringSlice ss);

#endif // MOSLIB_MEM_ALLOC_H
