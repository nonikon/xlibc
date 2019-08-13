#ifndef _XLIST_H_
#define _XLIST_H_

#include <stddef.h>

/* doubly-linked list, similar to C++ STL std::list */

typedef void (*xlist_destroy_cb)(void* pvalue);

typedef struct xlist_node
{
    struct xlist_node* prev;
    struct xlist_node* next;
    // char value[0];
} xlist_node_t, *xlist_iter_t;

typedef struct xlist
{
    size_t size;
    size_t val_size;        // element value size in 'xlist_node_t'
    xlist_destroy_cb destroy_cb;    // called when element destroy
    xlist_node_t head;
} xlist_t;

/* allocate memory for a 'xlist_t', 'val_size' is the size of element value.
   'cb' is called when element destroy, can be NULL, but it usually can't be NULL
   when value type is a pointer or contains a pointer. */
xlist_t* xlist_new(size_t val_size, xlist_destroy_cb cb);
/* release memory for a 'xlist_t'. */
void xlist_free(xlist_t* xl);

/* return the number of elements. */
#define xlist_size(xl)          ((xl)->size)
/* checks whether the container is empty. */
#define xlist_empty(xl)         ((xl)->size == 0)

/* return an iterator to the beginning. */
#define xlist_begin(xl)         ((xl)->head.next)
/* return an iterator to the end. */
#define xlist_end(xl)           (&(xl)->head)
/* return the next iterator of 'iter'. */
#define xlist_iter_next(iter)   ((iter)->next)
/* return a reverse iterator to the beginning. */
#define xlist_rbegin(xl)        ((xl)->head.prev)
/* return a reverse iterator to the end.  */
#define xlist_rend(xl)          (&(xl)->head)
/* return the next reverse iterator of 'iter'. */
#define xlist_riter_next(iter)  ((iter)->prev)

/* check whether an iterator is valid in a 'xlist_t', equal to "iter != xlist_end(xl)". */
#define xlist_iter_valid(xl, iter)  ((iter) != &(xl)->head)
/* return a pointer pointed to the element value of 'iter'. */
#define xlist_iter_value(iter)      ((void*)((iter) + 1))
/* return an iterator of an element value. */
#define xlist_value_iter(pvalue)    ((xlist_iter_t)(pvalue) - 1)
/* access the first element value (an pointer pointed to the value). */
#define xlist_front(xl)             xlist_iter_value(xlist_begin(xl))
/* access the last element value (an pointer pointed to the value). */
#define xlist_back(xl)              xlist_iter_value(xlist_rbegin(xl))

/* removes the element at 'iter', 'iter' MUST be valid.
   return an iterator following the last removed element. */
xlist_iter_t xlist_erase(xlist_t* xl, xlist_iter_t iter);
/* inserts an element BEFORE 'iter'.
   if 'pvalue' is not NULL, copy 'val_size' bytes memory of 'pvalue' into value,
   if 'pvalue' is NULL, leave value uninitialized. then, set it by yourself.
   return an iterator pointing to the inserted element. */
xlist_iter_t xlist_insert(xlist_t* xl, xlist_iter_t iter, const void* pvalue);
/* cut the element at 'iter', 'iter' MUST be valid.
   return a pointer pointed to the element value.
   'xlist_erase(xl, i)' is equal to 'xlist_cut_free(xl, xlist_cut(xl, i))'. */
void* xlist_cut(xlist_t* xl, xlist_iter_t iter);
/* destory an element which 'xlist_cut' returns ('xlist_destroy_cb' will be called). */
void xlist_cut_free(xlist_t* xl, void* pvalue);
/* clears the elements in a 'xlist_t'. */
void xlist_clear(xlist_t* xl);

/* set (copy) element value into 'iter'. */
#define xlist_set_value(xl, iter, pvalue) \
                                        memcpy(xlist_iter_value(iter), pvalue, (xl)->val_size)
/* inserts an element to the beginning. see 'xlist_insert'. */
#define xlist_push_front(xl, pvalue)    xlist_insert(xl, xlist_begin(xl), pvalue)
/* inserts an element to the end. see 'xlist_insert'. */
#define xlist_push_back(xl, pvalue)     xlist_insert(xl, xlist_end(xl), pvalue)
/* removes the first element. see 'xlist_erase'. */
#define xlist_pop_front(xl)             xlist_erase(xl, xlist_begin(xl))
/* removes the last element. see 'xlist_erase'. */
#define xlist_pop_back(xl)              xlist_erase(xl, xlist_rbegin(xl))
/* cut the first element. see 'xlist_cut'. */
#define xlist_cut_front(xl)             xlist_cut(xl, xlist_begin(xl))
/* cut the last element. see 'xlist_cut'. */
#define xlist_cut_back(xl)              xlist_cut(xl, xlist_rbegin(xl))

#endif // _XLIST_H_
