#ifndef _XARRAY_H_
#define _XARRAY_H_

/*
    sparse array. suitable for a series of continuous indexs.

    +-+-+---+---+---+---+
    | 0 | 1 | 2 |...|N-1|
    |   |nil|   |...|nil|
    +-+-+---+-+-+---+---+
      |       |
      |       +------------------+
      |                          |
      v                          v
    +-+-+---+---+---+---+      +-+-+---+---+---+---+
    | 0 | 1 | 2 |...|N-1|      | 0 | 1 | 2 |...|N-1|
    |nil|   |nil|...|nil|      |nil|   |nil|...|nil|
    +-+-+---+---+---+---+      +---+-+-+---+---+---+
          |                          |
          v                          v
        +-+--+                     +-+--+
        |User|                     |User|
        |Data|                     |Data|
        +----+                     +----+
        Index 0                   Index 2N+1
  */

#define XARRAY_BITS     4   // 4 or 8

#define XARRAY_BLOCK_SIZE   (1 << XARRAY_BITS)
#define XARRAY_MAX_DEPTH    (32 / XARRAY_BITS)  // + 32 % XARRAY_BITS != 0
#define XARRAY_MASK         (XARRAY_BLOCK_SIZE - 1)

typedef struct xarray       xarray_t;
typedef struct xarray_block xarray_block_t;
typedef struct xarray_iter  xarray_iter_t;

typedef void (*xarray_destroy_cb)(void*);

struct xarray_iter
{
    xarray_block_t* block;
    unsigned int    index;
    unsigned int    pos;    // = index & XARRAY_MASK
};

struct xarray_block
{
    xarray_iter_t  parent;  /* parent block. */
    unsigned short shift;
    unsigned short used;    /* how many value current block had used. */
    void*          values[XARRAY_BLOCK_SIZE];
};

struct xarray
{
    unsigned int      blocks;       /* how many block had allocated in xarray. */
                                    /* current just for DEBUG. */
    unsigned int      values;       /* how many value had allocated in xarray. */
                                    /* current just for DEBUG. */
    unsigned int      val_size;     /* size of value. */
    xarray_destroy_cb destroy_cb;   /* called when value is freed. */
    xarray_block_t    root;         /* root block. */
};

xarray_t* xarray_new(unsigned int val_size, xarray_destroy_cb cb);
void xarray_free(xarray_t* array);

/* set value at 'index', 'pvalue' can be 'NULL' (leave value uninitialized. set it by yourself).
   reuturn a pointer pointed to the new allocated value, return 'NULL' if out of memory. */
void* xarray_set(xarray_t* array, unsigned int index, void* pvalue);
/* get value at 'index'.
   return a pointer pointed to value, return 'NULL' if value is not set. */
void* xarray_get(xarray_t* array, unsigned int index);
/* remove value at 'index'. */
void xarray_unset(xarray_t* array, unsigned int index);
/* clear all values. */
void xarray_clear(xarray_t* array);

/* set 'iter' to the first iterator. */
void xarray_begin(xarray_t* array, xarray_iter_t* iter);
/* set 'iter' to the next iterator. */
void xarray_iter_next(xarray_iter_t* iter);
/* return if 'iter' is valid. */
#define xarray_iter_valid(iter) ((iter)->block != NULL)
/* return the index of 'iter', 'iter' MUST be valid. */
#define xarray_iter_index(iter) ((iter)->index)
/* return a poiniter pointed to the value at 'iter', 'iter' MUST be valid. */
#define xarray_iter_value(iter) ((iter)->block->values[(iter)->pos])

#endif // _XARRAY_H_