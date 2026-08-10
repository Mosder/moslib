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
//      size_t zerod = arr_put(array, i, item);
//
// If index is greater or equal to array length it will extend the length to the put item and it will
// zero all items between previously last item to the new item.
// It returns the count of items that were zerod that way.
//
// If you want to add more items than 1 at once, you can use these:
//
//      size_t index = arr_append_n(array, items, n_items);
//      size_t index = arr_push_n(array, items, n_items);
//      size_t index = arr_insert_n(array, i, items, n_items);
//      size_t zerod = arr_put_n(array, i, items, n_items);
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
//
// ----------------------------------------------------------------------------------------------------
// 
// To delete item from an array at specific index and retrieve it you can use:
//
//      T item = arr_pop_i(array, i);
//      T item = arr_pop(array); // same as arr_pop_i(array, arr_len(array)-1)
//
// No bounds checking, so it may return garbage or segfault.
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
