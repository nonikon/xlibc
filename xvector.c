#include <stdlib.h>
#include <string.h>

#include "xvector.h"

#define need_expand(xv, sz) \
            (xv)->size + sz > (xv)->capacity

static int capacity_expand(xvec_t* xv, size_t size)
{
    size_t new_cap = xv->capacity << 1; /* xs->capacity * 2; */
    void* new_data;

    size += xv->size;

    if (new_cap < size) new_cap = size;

    new_data = realloc(xv->data, xv->val_size * new_cap);

    if (new_data)
    {
        xv->capacity = new_cap;
        xv->data = new_data;
        return 0;
    }

    return -1;
}

static void* values_destroy(xvec_t* xv, size_t pos, int count)
{
    unsigned char* v = xvec_at(xv, pos);

    while (count-- > 0)
    {
        xv->destroy_cb((void*)v);
        v += xv->val_size;
    }

    return (void*)v;
}

xvec_t* xvec_init(xvec_t* xv, int capacity, size_t val_size,
            xvec_destroy_cb cb)
{
    xv->capacity = capacity > 0
            ? capacity : XVEC_DEFAULT_CAPACITY;
    /* no check 'xv->data' null or not */
    xv->data = malloc(val_size * xv->capacity);

    if (xv->data)
    {
        xv->destroy_cb  = cb;
        xv->size        = 0;
        xv->val_size    = val_size;
        return xv;
    }

    return NULL;
}

void xvec_destroy(xvec_t* xv)
{
    xvec_clear(xv);
    free(xv->data);
}

xvec_t* xvec_new(int capacity, size_t val_size, xvec_destroy_cb cb)
{
    xvec_t* r = malloc(sizeof(xvec_t));

    if (r)
    {
        if (xvec_init(r, capacity, val_size, cb))
            return r;
        free(r);
    }

    return NULL;
}

void xvec_free(xvec_t* xv)
{
    if (xv)
    {
        xvec_clear(xv);
        free(xv->data);
        free(xv);
    }
}

void xvec_append(xvec_t* xv, const void* pvalues, size_t count)
{
    if (need_expand(xv, count))
        capacity_expand(xv, count);

    if (pvalues)
        memcpy(xvec_at(xv, xv->size), pvalues,
                xv->val_size * count);

    xv->size += count;
}

void xvec_insert(xvec_t* xv,
        size_t pos, const void* pvalues, size_t count)
{
    if (need_expand(xv, count))
        capacity_expand(xv, count);

    /* >>> */
    memmove(xvec_at(xv, pos + count), xvec_at(xv, pos),
            xv->val_size * (xv->size - pos));

    if (pvalues)
        memcpy(xvec_at(xv, pos), pvalues,
                xv->val_size * count);

    xv->size += count;
}

void xvec_erase(xvec_t* xv, size_t pos, int count)
{
    unsigned char* v;

    if (count < 0 || pos + count >= xv->size)
    {
        if (xv->destroy_cb)
            values_destroy(xv, pos, xv->size - pos);

        xv->size = pos;
    }
    else
    {
        xv->size -= count;

        if (xv->destroy_cb)
            v = values_destroy(xv, pos, count);
        else
            v = xvec_at(xv, pos + count);

        /* <<< */
        memmove(xvec_at(xv, pos), v,
                xv->val_size * (xv->size - pos));
    }
}

void xvec_push_back(xvec_t* xv, const void* pvalue)
{
    if (need_expand(xv, 1))
        capacity_expand(xv, 1);

    if (pvalue)
        memcpy(xvec_at(xv, xv->size), pvalue, xv->val_size);

    ++xv->size;
}

void xvec_pop_back(xvec_t* xv)
{
    if (xv->destroy_cb)
        xv->destroy_cb(xvec_at(xv, xv->size));

    --xv->size;
}

void xvec_clear(xvec_t* xv)
{
    if (xv->destroy_cb)
        values_destroy(xv, 0, xv->size);

    xv->size = 0;
}