#ifndef _XRBTREE_H_
#define _XRBTREE_H_

#include <stddef.h>

/*
 * Red-black tree implementation is from linux kernel v5.3.7. (https://www.kernel.org/)
 * Can be used to implement 'set' and 'map' container.
 */

typedef struct xrbtree       xrbtree_t;
typedef struct xrbtree_node  xrbtree_node_t;
typedef struct xrbtree_node* xrbtree_iter_t;

typedef void (*xrbtree_destroy_cb)(void* pdata);
typedef int  (*xrbtree_compare_cb)(void* l, void* r);

struct xrbtree_node {
    unsigned long        rb_parent_color; // parent and color
    struct xrbtree_node* rb_right;
    struct xrbtree_node* rb_left;
    // each node have extra data which size is 'xrbtree_t.data_size'
};

struct xrbtree {
    xrbtree_compare_cb compare_cb;
    xrbtree_destroy_cb destroy_cb;
    size_t             data_size;
    size_t             size;
    xrbtree_node_t*    root;
};

/* allocate memory and initialize a 'xrbtree_t'.
 * 'compare_cb' is called when comparing two datas, can't be 'NULL'.
 * 'destroy_cb' is called when destroying an element, can be 'NULL'. */
xrbtree_t* xrbtree_new(size_t data_size, xrbtree_compare_cb compare_cb,
                xrbtree_destroy_cb destroy_cb);
/* release memory for a 'xrbtree_t'. */
void xrbtree_free(xrbtree_t* tr);

/* return the number of elements. */
#define xrbtree_size(tr)       ((tr)->size)
/* check whether the container is empty. */
#define xrbtree_empty(tr)      ((tr)->size == 0)

/* return an iterator to the beginning. */
xrbtree_iter_t xrbtree_begin(xrbtree_t* tr);
/* return the next iterator of 'iter'. */
xrbtree_iter_t xrbtree_iter_next(xrbtree_iter_t iter);
/* return a reverse iterator to the beginning. */
xrbtree_iter_t xrbtree_rbegin(xrbtree_t* tr);
/* return the next reverse iterator of 'iter'. */
xrbtree_iter_t xrbtree_riter_next(xrbtree_iter_t iter);

/* check whether an iterator is valid. */
#define xrbtree_iter_valid(iter)    ((iter) != NULL)
/* return a pointer pointed to the data of 'iter', 'iter' MUST be valid. */
#define xrbtree_iter_data(iter)     ((void*)((iter) + 1))
/* return an iterator of an element data. */
#define xrbtree_data_iter(pdata)    ((xrbtree_iter_t)(pdata) - 1)

/* insert an element with specific data, return an iterator to
 * the inserted element, return 'NULL' when out of memory.
 * if the data is already exist, do nothing an return it's iterator. */
xrbtree_iter_t xrbtree_insert(xrbtree_t* tr, const void* pdata);
/* find an element with specific data. return an iterator to the element with specific data,
 * return 'NULL' if not found. */
xrbtree_iter_t xrbtree_find(xrbtree_t* tr, const void* pdata);
/* remove an element at 'iter', iter' MUST be valid. */
void xrbtree_erase(xrbtree_t* tr, xrbtree_iter_t iter);
/* remove all elements in 'tr'. */
void xrbtree_clear(xrbtree_t* tr);

#endif // _XRBTREE_H_