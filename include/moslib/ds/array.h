// moslib/ds/array.h
// Implementation of dynamic array

// ======================================== QUICK USAGE GUIDE =========================================
// ----------------------------------------------------------------------------------------------------
//
// Define the dynamic array of type T as:
//
//      T *array = NULL;
//
// Then on the first usage of the array it's gonna be initialized.
// You can use arr_set_cap function right after definition to initalize to the specified capacity.
//
// ----------------------------------------------------------------------------------------------------
//
// You can access the items simply by array indexing:
//
//      T item = array[i];
//
// You can also get a pointer to an item with these:
//
//      T *p_item = arr_get(array, i);
//      T *p_item = arr_first(array);
//      T *p_item = arr_last(array);
//
// It will provide bounds checking - arr_get returning NULL when index is out of bounds
//                                 - both arr_first and arr_last returning NULL when array is empty
//
// ----------------------------------------------------------------------------------------------------
//
// To append/push to the array use:
//
//      size_t index = arr_append(array, item);
//
// or:
//
//      size_t index = arr_push(array, item);
//
// It returns the index of the newly appended/pushed item.
//
// To insert into specific index of the array and push items after it to the right use:
//
//      size_t index = arr_insert(array, i, item);
//
// It returns the index of the newly inserted item
// If index is greater or equal to array length then it functions identically to arr_append/arr_push.
//
// To put into specific index (and update if value exists) use:
//
//      size_t index = arr_put(array, i, item);
//
// It returns the index of the newly put item
// If index is greater or equal to array length it will extend the length to the put item and it will
// zero all items between previously last item to the new item.
//
// If you want to add more items than 1 at once, you can use these:
//
//      size_t index = arr_append_n(array, items, n_items);
//      size_t index = arr_push_n(array, items, n_items);
//      size_t index = arr_insert_n(array, i, items, n_items);
//      size_t index = arr_put_n(array, i, items, n_items);
//
// They work the same way as their counterparts without n, but accept an array of items instead.
//
// ----------------------------------------------------------------------------------------------------
//
// To concatenate two dynamic arrays of the same type use:
//
//      size_t index = arr_concat(array1, array2);
//
// First array will be the destination.
// Returns index of the first item of array2 in array1.
// If array 2 is NULL or has length 0 - returns (size_t)-1.
//
// ----------------------------------------------------------------------------------------------------
//
// To delete and retrieve the last item from an array use:
//
//      T item = arr_pop(array);
//
// No bounds checking, so it may return garbage or segfault if the array is empty.
//
// You can just delete items on given index with:
//
//      size_t deleted = arr_del(array, i);
//
// It will delete and item at given index and move items after to the left.
// Returns count of deleted items - it will be 0 if index is out of bounds.
// It also has an n variant:
//
//      size_t deleted = arr_del_n(array, i, n);
//
// It works the same way but deleted n consecutive items instead of 1.
//
// You can also remove from the front or the back:
//
//      size_t deleted = arr_del_front(array, n);
//      size_t deleted = arr_del_back(array, n);
//
// They work like arr_del_n without needing to specify the indices.
//
// ----------------------------------------------------------------------------------------------------
//
// You can use these to get a pointer to the previous or next item in array:
//
//      T *p_prev_item = arr_prev(array, p_item);
//      T *p_next_item = arr_next(array, p_item);
//
// When p_item is NULL it will return pointer to the last/first item.
// When there's no prev/next item - it will return NULL.
//
// ----------------------------------------------------------------------------------------------------
//
// To get array's current length use:
//
//      size_t length = arr_len(array);
//
// You can also set length:
//
//      size_t deleted = arr_set_len(array, new_len);
//
// If it's bigger than current length - it will zero new items.
// If it's smaller than current length - it will delete items.
// Returns amount of items deleted in this way.
//
// ----------------------------------------------------------------------------------------------------
//
// To get array's current capacity use:
//
//      size_t cap = arr_cap(array);
//
// You can also set capacity:
//
//      size_t new_cap = arr_set_cap(array, target_cap);
//
// When target_cap is lower than array length it will set capacity to array length.
// Returns the new capacity.
//
// ----------------------------------------------------------------------------------------------------
//
// To free the array from memory use:
//
//      arr_free(array);
//
// ----------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE =====================================

#include "moslib/mem/alloc.h"

#ifndef MOSLIB_DS_ARRAY_H
#define MOSLIB_DS_ARRAY_H

// Initial capacity to use in the array
#ifndef MOS_ARR_INIT_CAP
#define MOS_ARR_INIT_CAP 16
#endif // MOS_ARR_INIT_CAP

#ifndef MOS_FORCE_PREFIXES

#define arr_get mos_arr_get
#define arr_first mos_arr_first
#define arr_last mos_arr_last
#define arr_append mos_arr_append
#define arr_push mos_arr_push
#define arr_insert mos_arr_insert
#define arr_put mos_arr_put
#define arr_append_n mos_arr_append_n
#define arr_push_n mos_arr_push_n
#define arr_insert_n mos_arr_insert_n
#define arr_put_n mos_arr_put_n
#define arr_concat mos_arr_concat
#define arr_pop mos_arr_pop
#define arr_del mos_arr_del
#define arr_del_n mos_arr_del_n
#define arr_del_front mos_arr_del_front
#define arr_del_back mos_arr_del_back
#define arr_prev mos_arr_prev
#define arr_next mos_arr_next
#define arr_len mos_arr_len
#define arr_set_len mos_arr_set_len
#define arr_cap mos_arr_cap
#define arr_set_cap mos_arr_set_cap
#define arr_free mos_arr_free

#endif // MOS_FORCE_PREFIXES

// Get a pointer to an item at specific index in array
//
// Arguments:
//   arr
//     array to get an item from
//
//   i
//     index of the item to get
//
// Returns:
//   pointer to the item or NULL if i is out of bounds
#define mos_arr_get(arr, i) ((size_t)(i) < mos_arr_len(arr) ? (arr) + (size_t)(i) : NULL)

// Get a pointer to the first item in the array
//
// Arguments:
//   arr
//     array to get the first item from
//
// Returns:
//   pointer to the first item or NULL if array is empty
#define mos_arr_first(arr) (mos_arr_len(arr) == 0 ? NULL : (arr))

// Get a pointer to the last item in the array
//
// Arguments:
//   arr
//     array to get the last item from
//
// Returns:
//   pointer to the last item or NULL if array is empty
#define mos_arr_last(arr) (mos_arr_len(arr) == 0 ? NULL : (arr) + mos_arr_len(arr) - 1)

// clang-format off

// Append a given item to the array
//
// Arguments:
//   arr
//     array to append to
//
//   ...
//     item to append to the array
//     variadic to make things like (struct T){1,1} work properly
//
// Returns:
//   index of the appended item
#define mos_arr_push mos_arr_append
#define mos_arr_append(arr, ...) (                               \
    mos_arr_append_fn(&(arr), sizeof(*(arr)), MOS_ARR_INIT_CAP), \
    (arr)[mos_arr_len(arr) - 1] = __VA_ARGS__,                   \
    mos_arr_len(arr) - 1                                         \
)

// Insert a given item at the specific position in the array
// Move everything after it to the right
// If index is larger than length - it just appends the item
//
// Arguments:
//   arr
//     array to insert to
//
//   i
//     index to insert the item in
//
//   ...
//     item to insert to the array
//     variadic to make things like (struct T){1,1} work properly
//
// Returns:
//   index of the inserted item
#define mos_arr_insert(arr, i, ...) (                                   \
    (size_t)(i) >= mos_arr_len(arr) ? (                                 \
        mos_arr_append(arr, __VA_ARGS__)                                \
    ) : (                                                               \
        mos_arr_insert_fn(&(arr), i, sizeof(*(arr)), MOS_ARR_INIT_CAP), \
        (arr)[(size_t)(i)] = __VA_ARGS__,                               \
        (size_t)(i)                                                     \
    )                                                                   \
)

// Put a given item at the specific position in the array
// If index is within array bounds - it overwrites the item at given spot
// If index is larger than length - it puts the item at given index
//                                  and zeros items between previous last item and the new one
//
// Arguments:
//   arr
//     array to put to
//
//   i
//     index to put the item in
//
//   ...
//     item to put to the array
//     variadic to make things like (struct T){1,1} work properly
//
// Returns:
//   index of the put item
#define mos_arr_put(arr, i, ...) (                               \
    mos_arr_put_fn(&(arr), i, sizeof(*(arr)), MOS_ARR_INIT_CAP), \
    (arr)[(size_t)(i)] = __VA_ARGS__,                            \
    (size_t)(i)                                                  \
)

// clang-format on

// Append n given items to the array
//
// Arguments:
//   arr
//     array to append to
//
//   items
//     items to append to the array
//
//   n
//     count of items to append
//
// Returns:
//   index of the first appended item
#define mos_arr_push_n mos_arr_append_n
#define mos_arr_append_n(arr, items, n) mos_arr_append_n_fn(&(arr), items, n, sizeof(*(arr)), MOS_ARR_INIT_CAP)

// Insert n given items at the specific position in the array
// Move everything after them to the right
// If index is larger than length - it just appends the items
//
// Arguments:
//   arr
//     array to insert to
//
//   i
//     index to insert the items in
//
//   items
//     items to insert to the array
//
//   n
//     count of items to insert
//
// Returns:
//   index of the first inserted item
#define mos_arr_insert_n(arr, i, items, n) mos_arr_insert_n_fn(&(arr), i, items, n, sizeof(*(arr)), MOS_ARR_INIT_CAP)

// Put n given items at the specific position in the array
// If index is within array bounds - it overwrites the items at given spots
// If index is larger than length - it puts the items at given index
//                                  and zeros items between previous last item and the new ones
//
// Arguments:
//   arr
//     array to put to
//
//   i
//     index to put the items in
//
//   items
//     items to put to the array
//
//   n
//     count of items to put
//
// Returns:
//   index of the first put item
#define mos_arr_put_n(arr, i, items, n) mos_arr_put_n_fn(&(arr), i, items, n, sizeof(*(arr)), MOS_ARR_INIT_CAP)

// Concatenate two dynamic arrays of the same type
//
// Arguments:
//   arr1
//     array to concatenate to (destination)
//
//   arr2
//     array to concatenate (source)
//
// Returns:
//   index of the first new item in arr1 or (size_t)-1 if arr2 is NULL or has length 0
#define mos_arr_concat(arr1, arr2) mos_arr_concat_fn(&(arr1), arr2, sizeof(*(arr1)), MOS_ARR_INIT_CAP)

// Delete and retrieve the last item from the array
// No bounds checking
//
// Arguments:
//   arr
//     array to pop from
//
// Returns:
//   the popped item
#define mos_arr_pop(arr) (mos_arr_del_back(arr, 1), (arr)[mos_arr_len(arr)])

// Delete an item at specific index from the array
// Moves items after deleted one to the left
//
// Arguments:
//   arr
//     array to delete from
//
//   i
//     index of the item to delete
//
// Returns:
//   1 if item was deleted, 0 if it failed (e.g. out of bounds)
#define mos_arr_del(arr, i) mos_arr_del_fn(arr, i, sizeof(*(arr)))

// Delete n consecutive items at specific index from the array
// Moves items after deleted ones to the left
//
// Arguments:
//   arr
//     array to delete from
//
//   i
//     index of the first item to delete
//
//   n
//     count of items to delete
//
// Returns:
//   count of deleted items
#define mos_arr_del_n(arr, i, n) mos_arr_del_n_fn(arr, i, n, sizeof(*(arr)))

// Delete n first items from the array
// Moves items after deleted ones to the left
//
// Arguments:
//   arr
//     array to delete from
//
//   n
//     count of items to delete
//
// Returns:
//   count of deleted items
#define mos_arr_del_front(arr, n) mos_arr_del_front_fn(arr, n, sizeof(*(arr)))

// Delete n last items from the array
//
// Arguments:
//   arr
//     array to delete from
//
//   n
//     count of items to delete
//
// Returns:
//   count of deleted items
#define mos_arr_del_back(arr, n) mos_arr_del_back_fn(arr, n)

// Get the pointer to the previous item in the array from the pointer to the current one
// If pointer is NULL - gets the last item from the array
//
// Arguments:
//   arr
//     array to get previous item from
//
//   curr
//     pointer to the current item
//
// Returns:
//   pointer to the previous item or NULL if there's no more items
#define mos_arr_prev(arr, curr) ((!(arr) || !(curr)) ? mos_arr_last(arr) : ((curr) == mos_arr_first(arr) ? NULL : (arr) + mos_arr_p_diff(curr, arr) - 1))

// Get the pointer to the next item in the array from the pointer to the current one
// If pointer is NULL - gets the first item from the array
//
// Arguments:
//   arr
//     array to get next item from
//
//   curr
//     pointer to the current item
//
// Returns:
//   pointer to the next item or NULL if there's no more items
#define mos_arr_next(arr, curr) ((!(arr) || !(curr)) ? mos_arr_first(arr) : ((curr) == mos_arr_last(arr) ? NULL : (arr) + mos_arr_p_diff(curr, arr) + 1))

// Get the length of the array
//
// Arguments:
//   arr
//     array to get the length of
//
// Returns:
//   length of the array
extern size_t mos_arr_len(void *arr);

// Set the length of the array
// If it's bigger than current length - it will zero new items
// If it's smaller than current length - it will delete items
//
// Arguments:
//   arr
//     array to set the length of
//
//   len
//     length to set
//
// Returns:
//   count of the deleted items
#define mos_arr_set_len(arr, len) mos_arr_set_len_fn(&(arr), len, sizeof(*(arr)), MOS_ARR_INIT_CAP)

// Get the capacity of the array
//
// Arguments:
//   arr
//     array to get the capacity of
//
// Returns:
//   capacity of the array
extern size_t mos_arr_cap(void *arr);

// Set the capacity of the array
// If it's lower than current length - it will set the capacity equal to length
//
// Arguments:
//   arr
//     array to set the capacity of
//
//   cap
//     capacity to set
//
// Returns:
//   new capacity of the array
#define mos_arr_set_cap(arr, cap) mos_arr_set_cap_fn(&(arr), cap, sizeof(*(arr)))

// Free the array
//
// Arguments:
//   arr
//     array to free
extern void mos_arr_free(void *arr);

typedef struct {
    size_t len;
    size_t cap;
} MosArrHeader;

#define mos_arr_p_diff(p1, p2) (((char *)(p1) - (char*)(p2)) / sizeof(*(p2)))

// Function prototypes for macros
extern void mos_arr_append_fn(void *p_arr, size_t el_size, size_t init_cap);
extern void mos_arr_insert_fn(void *p_arr, size_t i, size_t el_size, size_t init_cap);
extern void mos_arr_put_fn(void *p_arr, size_t i, size_t el_size, size_t init_cap);
extern size_t mos_arr_append_n_fn(void *p_arr, void *items, size_t n, size_t el_size, size_t init_cap);
extern size_t mos_arr_insert_n_fn(void *p_arr, size_t i, void *items, size_t n, size_t el_size, size_t init_cap);
extern size_t mos_arr_put_n_fn(void *p_arr, size_t i, void *items, size_t n, size_t el_size, size_t init_cap);
extern size_t mos_arr_concat_fn(void *p_arr, void *arr2, size_t el_size, size_t init_cap);
extern size_t mos_arr_del_fn(void *arr, size_t i, size_t el_size);
extern size_t mos_arr_del_n_fn(void *arr, size_t i, size_t n, size_t el_size);
extern size_t mos_arr_del_front_fn(void *arr, size_t n, size_t el_size);
extern size_t mos_arr_del_back_fn(void *arr, size_t n);
extern size_t mos_arr_set_len_fn(void *p_arr, size_t len, size_t el_size, size_t init_cap);
extern size_t mos_arr_set_cap_fn(void *p_arr, size_t cap, size_t el_size);

#endif // MOSLIB_DS_ARRAY_H
