#ifndef _XSET_H_
#define _XSET_H_

#include <stddef.h>

/*
 * An associative container that contains a sorted set of unique
 * objects of type Key, similar to C++ STL std::set.
 * Red-black tree implementation is from linux kernel v5.3.7. (https://www.kernel.org/)
 */

typedef void (*xset_destroy_cb)(void* pvalue);
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

xset_t* xset_new(size_t key_size,
				xset_compare_cb compare_cb,
				xset_destroy_cb destroy_cb);
void xset_free(xset_t* xs);

#define xset_size(xs)           ((xs)->size)
#define xset_empty(xs)          ((xs)->size == 0)

xset_iter_t xset_begin(xset_t* xs);
xset_iter_t xset_iter_next(xset_iter_t iter);
xset_iter_t xset_rbegin(xset_t* xs);
xset_iter_t xset_riter_next(xset_iter_t iter);

#define xset_iter_valid(iter)   ((iter) != NULL)
#define xset_iter_value(iter)   ((void*)((iter) + 1))

xset_iter_t xset_insert(xset_t* xs, const void* pvalue);
xset_iter_t xset_find(xset_t* xs, const void* pvalue);
/* 'iter' MUST be valid. */
void xset_erase(xset_t* xs, xset_iter_t iter);
void xset_clear(xset_t* xs);

#endif // _XSET_H_