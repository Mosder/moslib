// moslib/ds/heap.h
// Implementation of heap

// ======================================= QUICK USAGE GUIDE =======================================
// -------------------------------------------------------------------------------------------------
//
// Define a heap of type T as:
//
//      T *min_heap = heap_new(HEAP_MIN);
//      T *max_heap = heap_new(HEAP_MAX);
//
// By default it will compare the values byte by byte.
//
// You can change how it's compared by using .cmp optional argument, e.g.:
//
//      int heap_cmp(const void *el1, const void *el2) {
//          char *e1 = *(char **)el1;
//          char *e2 = *(char **)el2;
//          return strcmp(e1, e2);
//      }
//
//      char **min_heap = heap_new(HEAP_MIN, .cmp = heap_cmp);
//
// The compare function should return a negative number when the first value is "lesser" than the
// second and a positive number when the the first value is "greater" than the second.
//
// You can also define it just by assigning a NULL:
//
//      T *min_heap = NULL;
//
// It will automatically initialize as a min heap with the default cmp function.
//
// -------------------------------------------------------------------------------------------------
//
// To add an element to the heap, use:
//
//      heap_add(heap, element);
//
// -------------------------------------------------------------------------------------------------
//
// To get the top of the heap, use:
//
//      T *top = heap_top(heap);
//
// It will return the pointer to the top of the heap or NULL if the heap has no elements.
//
// -------------------------------------------------------------------------------------------------
//
// To pop from the top of the heap, use:
//
//      T top = heap_pop(heap);
//
// If the heap is empty, it will return a value with all bytes set to 0.
//
// -------------------------------------------------------------------------------------------------
//
// To delete the top element from a heap, use:
//
//      int deleted = heap_del_top(heap);
//
// It will return 1 if the top existed and was deleted, 0 otherwise.
//
// -------------------------------------------------------------------------------------------------
//
// To get heap's size, use:
//
//      size_t size = heap_size(heap);
//
// -------------------------------------------------------------------------------------------------
//
// To free the heap, use:
//
//      heap_free(heap);
//
// -------------------------------------------------------------------------------------------------
// ===================================== END OF QUICK USAGE GUIDE ==================================

#include <stddef.h>

#ifndef MOSLIB_DS_HEAP_H
#define MOSLIB_DS_HEAP_H

#ifndef MOS_FORCE_PREFIXES

#define heap_new mos_heap_new
#define HEAP_MIN MOS_HEAP_MIN
#define HEAP_MAX MOS_HEAP_MAX
#define heap_add mos_heap_add
#define heap_top mos_heap_top
#define heap_pop mos_heap_pop
#define heap_del_top mos_heap_del_top
#define heap_size mos_heap_size
#define heap_free mos_heap_free

#endif // MOS_FORCE_PREFIXES

typedef struct {
    enum { MOS_HEAP_MIN, MOS_HEAP_MAX } type;
    int (*cmp)(const void *el1, const void *el2);
} MosHeapNewArgs;

// Create a new heap
//
// Arguments:
//   type
//     type of heap (min/max)
//
// Optional arguments:
//   .cmp
//     user defined compare function
//
// Returns:
//   pointer to the new hashmap
#define mos_heap_new(...) mos_heap_new_fn((MosHeapNewArgs){.type = __VA_ARGS__})

// Add an element to the heap
//
// Arguments:
//   heap
//     heap to add into
//
//   ...
//     element to add into heap
//     variadic to make things like (struct T){1,1} work properly
#define mos_heap_add(heap, ...)                                                                    \
    (mos_heap_init(&(heap), sizeof(*(heap))),                                                      \
     *(heap) = __VA_ARGS__,                                                                        \
     mos_heap_add_fn(&(heap), sizeof(*(heap))))

// Get a pointer to the top element in heap
//
// Arguments:
//   heap
//     heap to get the top element from
//
// Returns:
//   pointer to the top element or NULL if heap is empty
#define mos_heap_top(heap) (mos_heap_size(heap) ? (heap) + 1 : NULL)

// Pop the top of the heap
//
// Arguments:
//   heap
//     heap to pop the top element from
//
// Returns:
//  the popped element or 0 if the heap is empty
#define mos_heap_pop(heap)                                                                         \
    (mos_heap_init(&(heap), sizeof(*(heap))), mos_heap_pop_fn(heap, sizeof(*(heap))), *(heap))

// Delete the top element of the heap
//
// Arguments:
//   heap
//     heap to delete the top element from
//
// Returns:
//  1 if the top existed and was deleted, 0 otherwise
#define mos_heap_del_top(heap) mos_heap_del_top_fn(heap, sizeof(*heap))

// Get the count of elements in the heap
//
// Arguments:
//   heap
//     heap to get the count of elements from
extern size_t mos_heap_size(const void *heap);

// Free the heap
//
// Arguments:
//   heap
//     heap to free
extern void mos_heap_free(void *heap);

// Function prototypes for macros
extern void mos_heap_init(void *p_heap, size_t el_size);
extern void *mos_heap_new_fn(MosHeapNewArgs args);
extern void mos_heap_add_fn(void *p_heap, size_t el_size);
extern void mos_heap_pop_fn(void *heap, size_t el_size);
extern int mos_heap_del_top_fn(void *heap, size_t el_size);

#endif // MOSLIB_DS_HEAP_H
