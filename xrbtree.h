#ifndef _XRBTREE_H_
#define _XRBTREE_H_

#include <stddef.h>

/* red-black tree implementation is from linux kernel v5.3.7. (https://www.kernel.org/) */

#ifdef HAVE_XCONFIG_H
# include "xconfig.h"
#else
/* cache can decrease memory allocation. node will be put into cache
 * when it being erased, and next insertion will pop one node from
 * cache. define 'XRBT_ENABLE_CACHE=1' to enable it. */
# ifndef XRBT_ENABLE_CACHE
#  define XRBT_ENABLE_CACHE 0
# endif
#endif

typedef struct xrbt         xrbt_t;
typedef struct xrbt_node    xrbt_node_t;
typedef struct xrbt_node*   xrbt_iter_t;

typedef void (*xrbt_destroy_cb)(void* pdata);
typedef int  (*xrbt_compare_cb)(void* l, void* r);

struct xrbt_node {
    unsigned long       rb_parent_color; // parent and color
    struct xrbt_node*   rb_right;
    struct xrbt_node*   rb_left;
    // each node have extra data which size is 'xrbt_t.data_size'
};

struct xrbt {
    xrbt_compare_cb compare_cb;
    xrbt_destroy_cb destroy_cb;
    size_t          data_size;
    size_t          size;
#if XRBT_ENABLE_CACHE
    xrbt_node_t*    cache;
#endif
    xrbt_node_t*    root;
};

/* initialize a 'xrbt_t'.
 * 'compare_cb' is called when comparing two datas, can't be 'NULL'.
 * 'destroy_cb' is called when destroying an element, can be 'NULL'. */
xrbt_t* xrbt_init(xrbt_t* tr, size_t data_size,
            xrbt_compare_cb compare_cb, xrbt_destroy_cb destroy_cb);
/* destroy a 'xrbt_t' which has called 'xrbt_init'. */
void xrbt_destroy(xrbt_t* tr);

/* allocate memory for a 'xrbt_t' and initialize it. */
xrbt_t* xrbt_new(size_t data_size, xrbt_compare_cb compare_cb,
            xrbt_destroy_cb destroy_cb);
/* release memory for a 'xrbt_t' which 'xrbt_new' returns. */
void xrbt_free(xrbt_t* tr);

#if XRBT_ENABLE_CACHE
/* free all cache nodes in a 'xrbt_t'. */
void xrbt_cache_free(xrbt_t* tr);
#endif

/* return the number of elements. */
#define xrbt_size(tr)       ((tr)->size)
/* check whether the container is empty. */
#define xrbt_empty(tr)      ((tr)->size == 0)
/* return an iterator to the end. */
#define xrbt_end(xh)        NULL
/* return a reverse iterator to the end.  */
#define xrbt_rend(xh)       NULL

/* return an iterator to the beginning. */
xrbt_iter_t xrbt_begin(xrbt_t* tr);
/* return the next iterator of 'iter'. */
xrbt_iter_t xrbt_iter_next(xrbt_iter_t iter);
/* return a reverse iterator to the beginning. */
xrbt_iter_t xrbt_rbegin(xrbt_t* tr);
/* return the next reverse iterator of 'iter'. */
xrbt_iter_t xrbt_riter_next(xrbt_iter_t iter);

/* check whether an iterator is valid. */
#define xrbt_iter_valid(iter)   ((iter) != NULL)
/* return a pointer pointed to the data of 'iter', 'iter' MUST be valid. */
#define xrbt_iter_data(iter)    ((void*)((iter) + 1))
/* return an iterator of an element data. */
#define xrbt_data_iter(pdata)   ((xrbt_iter_t)(pdata) - 1)

/* insert an element with specific data, return an iterator to
 * the inserted element, return 'NULL' when out of memory.
 * if the data is already exist, do nothing an return it's iterator. */
#define xrbt_insert(tr, pdata)  xrbt_insert_ex(tr, pdata, (tr)->data_size)
/* similar to 'xrbt_insert', but useful when we don't want to init all 'data_size',
 * just init the <key> (which size is 'ksz'), and set <value> by yourself later. */
xrbt_iter_t xrbt_insert_ex(xrbt_t* tr, const void* pdata, size_t ksz);
/* find an element with specific data. return an iterator to the element with specific data,
 * return 'NULL' if not found. */
xrbt_iter_t xrbt_find(xrbt_t* tr, const void* pdata);
/* remove an element at 'iter', 'iter' MUST be valid. */
void xrbt_erase(xrbt_t* tr, xrbt_iter_t iter);
/* remove all elements (no cache) in 'tr'. */
void xrbt_clear(xrbt_t* tr);

/* find an element with specific data. return a pointer to the element
 * with specific data, return 'XRBT_INVALID_DATA' if not found.
 * the return value can call 'xrbt_data_iter' to get it's iterator. */
#define xrbt_find_data(tr, pdata)   xrbt_iter_data(xrbt_find(tr, pdata))
/* remove an element, 'pdata' should be the return value of 'xrbt_find_data'
 * and not equal to 'XRBT_INVALID_DATA'. */
#define xrbt_erase_data(tr, pdata)  xrbt_erase(tr, xrbt_data_iter(pdata))

#define XRBT_INVALID_DATA    xrbt_iter_data((xrbt_iter_t)0)

#endif // _XRBTREE_H_
