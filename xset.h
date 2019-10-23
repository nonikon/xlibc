#ifndef _XSET_H_
#define _XSET_H_

#include <stddef.h>

/*
 * An associative container that contains a sorted set of unique
 * objects of type Key, similar to C++ STL std::set.
 * Red-black tree implementation is from linux kernel v5.3.7. (https://www.kernel.org/)
 */

typedef void (*xset_destroy_cb)(void* pkey);
typedef int (*xset_compare_cb)(void* l, void* r);

typedef struct rb_node {
    unsigned long   __rb_parent_color; // parent and color

    struct rb_node* rb_right;
    struct rb_node* rb_left;
    // char rb_key[0];
} xset_node_t, *xset_iter_t;

typedef struct xset {
    xset_compare_cb compare_cb;
    xset_destroy_cb destroy_cb;
    size_t          key_size;
    size_t          size;
    xset_node_t*    root;
} xset_t;

/* allocate memory and initialize a 'xset_t'.
 * 'compare_cb' is called when comparing two keys, can't be 'NULL'.
 * 'destroy_cb' is called when destroying an element, can be 'NULL'. */
xset_t* xset_new(size_t key_size, xset_compare_cb compare_cb,
                xset_destroy_cb destroy_cb);
/* release memory for a 'xset_t'. */
void xset_free(xset_t* xs);

/* return the number of elements. */
#define xset_size(xs)           ((xs)->size)
/* check whether the container is empty. */
#define xset_empty(xs)          ((xs)->size == 0)

/* return an iterator to the beginning. */
xset_iter_t xset_begin(xset_t* xs);
/* return the next iterator of 'iter'. */
xset_iter_t xset_iter_next(xset_iter_t iter);
/* return a reverse iterator to the beginning. */
xset_iter_t xset_rbegin(xset_t* xs);
/* return the next reverse iterator of 'iter'. */
xset_iter_t xset_riter_next(xset_iter_t iter);

/* check whether an iterator is valid. */
#define xset_iter_valid(iter)   ((iter) != NULL)
/* return a pointer pointed to the key of 'iter', 'iter' MUST be valid. */
#define xset_iter_key(iter)     ((void*)((iter) + 1))
/* return an iterator of an element key. */
#define xset_key_iter(pkey)     ((xset_iter_t)(pkey) - 1)

/* insert an element with specific key, return an iterator to
 * the inserted element, return 'NULL' when out of memory.
 * if the key is already exist, do nothing an return it's iterator. */
xset_iter_t xset_insert(xset_t* xs, const void* pkey);
/* find an element with specific key. return an iterator to the element with specific key,
 * return 'NULL' if not found. */
xset_iter_t xset_find(xset_t* xs, const void* pkey);
/* remove an element at 'iter', iter' MUST be valid. */
void xset_erase(xset_t* xs, xset_iter_t iter);
/* remove all elements in 'xs'. */
void xset_clear(xset_t* xs);

#endif // _XSET_H_