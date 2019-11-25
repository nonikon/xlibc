#ifndef _XHASH_H_
#define _XHASH_H_

#include <stddef.h>

/* Hash table, logic base on java hash table. */

#ifndef XHASH_DEFAULT_SIZE
#define XHASH_DEFAULT_SIZE          64 // MUST be 2^n
#endif

#ifndef XHASH_DEFAULT_LOADFACTOR
#define XHASH_DEFAULT_LOADFACTOR    75 // percent
#endif

typedef struct xhash       xhash_t;
typedef struct xhash_node  xhash_node_t;
typedef struct xhash_node* xhash_iter_t;

typedef void     (*xhash_destroy_cb)(void* pdata);
typedef unsigned (*xhash_hash_cb)(void* pdata);
typedef int      (*xhash_equal_cb)(void* l, void* r);

struct xhash_node
{
    struct xhash_node* prev;
    struct xhash_node* next;
    unsigned           hash;
    // char data[0];
};

struct xhash
{
    xhash_hash_cb    hash_cb;
    xhash_equal_cb   equal_cb;
    xhash_destroy_cb destroy_cb;
    size_t           bkt_size;    // buckets size
    size_t           data_size;
    size_t           size;        // element (node) count
    size_t           loadfactor;
    xhash_node_t**   buckets;
};

/* allocate memory and initialize a 'xhash_t'.
 * 'size' is the init bucket size, MUST be 2^n or -1 (means default).
 * 'hash_cb' is used to get hash code of an element, can't be 'NULL'.
 * 'equal_cb' is used to check the equals of two elements, can't be 'NULL'.
 * 'destroy_cb' is called when destroying an element, can be 'NULL'. */
xhash_t* xhash_new(int size, size_t data_size, xhash_hash_cb hash_cb,
            xhash_equal_cb equal_cb, xhash_destroy_cb destroy_cb);
/* release memory for a 'xhash_t'. */
void xhash_free(xhash_t* xh);

/* set loadfactor of 'xh', 'factor' is an interger which
 * standfor loadfactor percent. */
#define xhash_set_loadfactor(xh, factor) \
                                (xh)->loadfactor = factor

/* return the number of elements. */
#define xhash_size(xh)          ((xh)->size)
/* check whether the container is empty. */
#define xhash_empty(xh)         ((xh)->size == 0)

/* return an iterator to the beginning. */
xhash_iter_t xhash_begin(xhash_t* xh);
/* return the next iterator of 'iter'. */
xhash_iter_t xhash_iter_next(xhash_t* xh, xhash_iter_t iter);

/* check whether an iterator is valid. */
#define xhash_iter_valid(iter)  ((iter) != NULL)
/* return a pointer pointed to the data of 'iter', 'iter' MUST be valid. */
#define xhash_iter_data(iter)   ((void*)((iter) + 1))
/* return an iterator of an element data. */
#define xhash_data_iter(pdata)  ((xhash_iter_t)(pdata) - 1)

/* insert an element with specific data, return an iterator to
 * the inserted element, return 'NULL' when out of memory.
 * if the data is already exist, do nothing an return it's iterator. */
xhash_iter_t xhash_put(xhash_t* xh, const void* pdata);
/* find an element with specific data. return an iterator to the element with specific data,
 * return 'NULL' if not found. */
xhash_iter_t xhash_get(xhash_t* xh, const void* pdata);
/* remove an element at 'iter', 'iter' MUST be valid. */
void xhash_remove(xhash_t* xh, xhash_iter_t iter);
/* remove all elements in 'xh'. */
void xhash_clear(xhash_t* xh);

/* Some helper hash function, can be used in 'xhash_hash_cb'. */

/* Aim to protect against poor hash functions by adding logic here
 * - logic taken from java 1.4 hashtable source. */
static inline unsigned xhash_improve_hash(unsigned h)
{
    h += ~(h << 9);
    h ^= ((h >> 14) | (h << 18)); /* >>> */
    h += (h << 4);
    h ^= ((h >> 10) | (h << 22)); /* >>> */
    return h;
}

#if 0
/* Basic string hash function, from Java standard String.hashCode(). */
static inline unsigned xhash_string_hash(const char* s)
{
    unsigned h = 0;
    int m = 1;
    while (*s)
    {
        h += (*s++) * m;
        m = (m << 5) - 1; /* m *= 31 */
    }
    return h;
}
#endif

/* Basic string hash function, from Python's str.__hash__(). */
static inline unsigned xhash_string_hash(const char *s)
{
    const unsigned char *cp = (const unsigned char *)s;
    unsigned h = *cp << 7;
    while (*cp)
        h = (1000003 * h) ^ *cp++;
    /* This conversion truncates the length of the string, but that's ok. */
    h ^= (unsigned)(cp - (const unsigned char *)s);
    return h;
}

#endif // _XHASH_H_