#ifndef _XRB_TREE_H_
#define _XRB_TREE_H_

#include <stddef.h>

/*
 * Red-black tree implementation is from linux kernel v5.3.7. (https://www.kernel.org/)
 * Can be used to implement 'set' and 'map' container.
 */

typedef void (*xrb_destroy_cb)(void* pdata);
typedef int (*xrb_compare_cb)(void* l, void* r);

typedef struct rb_node {
    unsigned long   rb_parent_color; // parent and color
    struct rb_node* rb_right;
    struct rb_node* rb_left;
    // char rb_data[0]; // each node have extra data which size is 'xrb_tree_t.data_size'
} xrb_node_t, *xrb_iter_t;

typedef struct xrb_tree {
    xrb_compare_cb  compare_cb;
    xrb_destroy_cb  destroy_cb;
    size_t          data_size;
    size_t          size;
    xrb_node_t*     root;
} xrb_tree_t;

/* allocate memory and initialize a 'xrb_tree_t'.
 * 'compare_cb' is called when comparing two datas, can't be 'NULL'.
 * 'destroy_cb' is called when destroying an element, can be 'NULL'. */
xrb_tree_t* xrb_tree_new(size_t data_size, xrb_compare_cb compare_cb,
                xrb_destroy_cb destroy_cb);
/* release memory for a 'xrb_tree_t'. */
void xrb_tree_free(xrb_tree_t* tr);

/* return the number of elements. */
#define xrb_tree_size(tr)       ((tr)->size)
/* check whether the container is empty. */
#define xrb_tree_empty(tr)      ((tr)->size == 0)

/* return an iterator to the beginning. */
xrb_iter_t xrb_tree_begin(xrb_tree_t* tr);
/* return the next iterator of 'iter'. */
xrb_iter_t xrb_iter_next(xrb_iter_t iter);
/* return a reverse iterator to the beginning. */
xrb_iter_t xrb_tree_rbegin(xrb_tree_t* tr);
/* return the next reverse iterator of 'iter'. */
xrb_iter_t xrb_riter_next(xrb_iter_t iter);

/* check whether an iterator is valid. */
#define xrb_iter_valid(iter)    ((iter) != NULL)
/* return a pointer pointed to the data of 'iter', 'iter' MUST be valid. */
#define xrb_iter_data(iter)     ((void*)((iter) + 1))
/* return an iterator of an element data. */
#define xrb_data_iter(pdata)    ((xrb_iter_t)(pdata) - 1)

/* insert an element with specific data, return an iterator to
 * the inserted element, return 'NULL' when out of memory.
 * if the data is already exist, do nothing an return it's iterator. */
xrb_iter_t xrb_tree_insert(xrb_tree_t* tr, const void* pdata);
/* find an element with specific data. return an iterator to the element with specific data,
 * return 'NULL' if not found. */
xrb_iter_t xrb_tree_find(xrb_tree_t* tr, const void* pdata);
/* remove an element at 'iter', iter' MUST be valid. */
void xrb_tree_erase(xrb_tree_t* tr, xrb_iter_t iter);
/* remove all elements in 'tr'. */
void xrb_tree_clear(xrb_tree_t* tr);

#endif // _XRB_TREE_H_