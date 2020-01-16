#ifndef _XVECTOR_H_
#define _XVECTOR_H_

#include <stddef.h>

/* dynamic contiguous array, similar to C++ STL std::vector library. */

#ifndef XVEC_DEFAULT_CAPACITY
#define XVEC_DEFAULT_CAPACITY   16
#endif

typedef struct xvec xvec_t;

typedef void (*xvec_destroy_cb)(void* pvalue);

struct xvec
{
    xvec_destroy_cb destroy_cb;
    size_t          capacity;
    size_t          size;
    size_t          val_size;
    unsigned char*  data;
};

/* allocate memory for a 'xvec_t', 'val_size' is the size of element value.
 * 'cb' is called when element destroy, can be NULL, but it usually can't be NULL
 * when value type is a pointer or contains a pointer. */
xvec_t* xvec_new(int capacity, size_t val_size, xvec_destroy_cb cb);
/* release memory for a 'xvec_t'. */
void xvec_free(xvec_t* xv);

/* access the value at index 'i'. */
#define xvec_at(xv, i)      ((void*)((xv)->data + (xv)->val_size * (i)))
/* direct access to the underlying array. */
#define xvec_data(xv)       ((void*)(xv)->data)
/* checks whether the container is empty. */
#define xvec_empty(xv)      ((xv)->size == 0)
/* return the number of elements. */
#define xvec_size(xv)       ((xv)->size)
/* return the number of values that can be held in currently allocated storage. */
#define xvec_capacity(xv)   ((xv)->capacity)
/* access the first value. */
#define xvec_front(xv)      xvec_at(xv, 0)
/* access the last value. */
#define xvec_back(xv)       xvec_at(xv, (xv)->size - 1)

/* append 'count' values to the end. */
void xvec_append(xvec_t* xv, const void* pvalues, size_t count);
/* insert 'count' values to at 'pos'. */
void xvec_insert(xvec_t* xv, size_t pos, const void* pvalues, size_t count);
/* remove 'count' values to at 'pos'. */
void xvec_erase(xvec_t* xv, size_t pos, int count);
/* remove all values. */
void xvec_clear(xvec_t* xv);

/* prepend 'count' values to the begin. */
#define xvec_prepend(xv, pvalues, count)    xvec_insert(xv, 0, pvalues, count)

/* append one value to the end. */
#define xvec_push_back(xv, pvalue)          xvec_append(xv, pvalue, 1)
/* append one value to the begin. */
#define xvec_push_front(xv, pvalue)         xvec_prepend(xv, pvalue, 1)

/* remove the first value. */
void xvec_pop_back(xvec_t* xv);
/* remove the last value. */
void xvec_pop_front(xvec_t* xv);

#endif // _XVECTOR_H_