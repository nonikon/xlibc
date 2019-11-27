#ifndef _XARRAY_H_
#define _XARRAY_H_

#include <stddef.h>

/*
 * sparse array (or "radix tree"). suitable for a series of continuous indexs.
 *
 * +-+-+---+---+---+---+
 * | 0 | 1 | 2 |...|N-1|
 * |   |nil|   |...|nil| (this is a block)
 * +-+-+---+-+-+---+---+
 *   |       |
 *   |       +------------------+
 *   |                          |
 *   v                          v
 * +-+-+---+---+---+---+      +-+-+---+---+---+---+
 * | 0 | 1 | 2 |...|N-1|      | 0 | 1 | 2 |...|N-1|
 * |nil|   |nil|...|nil|      |nil|   |nil|...|nil|
 * +-+-+---+---+---+---+      +---+-+-+---+---+---+
 *       |                          |
 *       v                          v
 *     +-+--+                     +-+--+
 *     |Node|                     |Node|
 *     |Data|                     |Data| (this is a node)
 *     +----+                     +----+
 *     Index 0                   Index 2N+1
 */

#define XARRAY_BITS         4   // 1, 2, 4, 8

#define XARRAY_BLOCK_SIZE   (1 << XARRAY_BITS)
#define XARRAY_MASK         (XARRAY_BLOCK_SIZE - 1)

typedef unsigned int xuint;
// typedef unsigned long xuint;

typedef struct xarray       xarray_t;
typedef struct xarray_block xarray_block_t;
typedef struct xarray_node  xarray_node_t;
typedef struct xarray_node* xarray_iter_t;

typedef void (*xarray_destroy_cb)(void*);

struct xarray_node
{
    xarray_block_t* block;
    xuint           index;
    // char data[0];
};

struct xarray_block
{
    xarray_block_t* parent_block;
    unsigned char   parent_pos;
    unsigned char   shift;
    unsigned char   used;    /* how many values current block had used. */
    void*           values[XARRAY_BLOCK_SIZE];
};

struct xarray
{
    size_t            blocks;       /* how many block had allocated in xarray. */
                                    /* current just for DEBUG. */
    size_t            values;       /* how many value had allocated in xarray. */
                                    /* current just for DEBUG. */
    size_t            val_size;
    xarray_destroy_cb destroy_cb;   /* called when value is removed. */
    xarray_block_t    root;         /* root block. */
};

xarray_t* xarray_new(size_t val_size, xarray_destroy_cb cb);
void xarray_free(xarray_t* array);

/* set value at 'index', 'pvalue' can be 'NULL' (means set it later, just allocate memory).
 * if 'index' has already been set, it will be unset (call 'destroy_cb') first.
 * reuturn a pointer pointed to new allocated value. return 'NULL' if out of memory. 
 * the return value can call 'xarray_value_iter' to get it's iterator. */
void* xarray_set(xarray_t* array, xuint index, void* pvalue);
/* get value at 'index'.
 * return a pointer pointed to value, return 'NULL' if value has not been set.
 * the return value can call 'xarray_value_iter' to get it's iterator. */
void* xarray_get(xarray_t* array, xuint index);
/* remove value at 'index'. */
void xarray_unset(xarray_t* array, xuint index);
/* clear all values. */
void xarray_clear(xarray_t* array);

/* return an iterator to the beginning. */
xarray_iter_t xarray_begin(xarray_t* array);
/* return the next iterator of 'iter'. */
xarray_iter_t xarray_iter_next(xarray_iter_t iter);

/* return if 'iter' is valid. */
#define xarray_iter_valid(iter)   ((iter) != NULL)
/* return the index of 'iter', 'iter' MUST be valid. */
#define xarray_iter_index(iter)   ((iter)->index)
/* return a poiniter pointed to the value at 'iter', 'iter' MUST be valid. */
#define xarray_iter_value(iter)   ((void*)((iter) + 1))
/* return an iterator of an element value. */
#define xarray_value_iter(pvalue) ((xarray_iter_t)(pvalue) - 1)

#endif // _XARRAY_H_