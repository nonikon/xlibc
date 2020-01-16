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

xvec_t* xvec_new(int capacity, size_t val_size, xvec_destroy_cb cb)
{
    xvec_t* r = malloc(sizeof(xvec_t));

    if (r)
    {
        r->capacity = capacity > 0
                ? capacity : XVEC_DEFAULT_CAPACITY;
        r->data = malloc(val_size * r->capacity);

        if (r->data)
        {
            r->destroy_cb = cb;
            r->size = 0;
            r->val_size = val_size;
            return r;
        }

        free(r);
        return NULL;
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
        memcpy(xvec_at(xv, xv->size),
                pvalues, xv->val_size * count);

    xv->size += count;
}

void xvec_insert(xvec_t* xv,
        size_t pos, const void* pvalues, size_t count)
{
    if (need_expand(xv, count))
        capacity_expand(xv, count);

    /* >>> */
    memmove(xvec_at(xv, pos + count),
            xvec_at(xv, pos),
            xv->val_size * (xv->size - pos));

    if (pvalues)
        memcpy(xvec_at(xv, pos),
                pvalues, xv->val_size * count);

    xv->size += count;
}

void xvec_erase(xvec_t* xv, size_t pos, int count)
{
    unsigned char* v;

    if (count < 0 || pos + count >= xv->size)
    {
        if (xv->destroy_cb)
        {
            count = xv->size - pos;
            v = xvec_at(xv, pos);

            while (count-- > 0)
            {
                xv->destroy_cb((void*)v);
                v += xv->val_size;
            }
        }

        xv->size = pos;
    }
    else
    {
        if (xv->destroy_cb)
        {
            v = xvec_at(xv, pos);

            while (count-- > 0)
            {
                xv->destroy_cb((void*)v);
                v += xv->val_size;
            }
        }

        xv->size -= count;
        /* <<< */
        memmove(xvec_at(xv, pos),
                xvec_at(xv, pos + count),
                xv->val_size * (xv->size - pos));
    }
}

void xvec_pop_back(xvec_t* xv)
{
    --xv->size;

    if (xv->destroy_cb)
        xv->destroy_cb(xvec_at(xv, xv->size));
}

void xvec_pop_front(xvec_t* xv)
{
    if (xv->destroy_cb)
        xv->destroy_cb(xvec_at(xv, 0));

    --xv->size;
    /* <<< */
    memmove(xvec_at(xv, 0),
            xvec_at(xv, 1),
            xv->val_size * xv->size);
}

void xvec_clear(xvec_t* xv)
{
    unsigned char* v = xv->data;

    if (xv->destroy_cb)
    {
        while (xv->size-- > 0)
        {
            xv->destroy_cb((void*)v);
            v += xv->val_size;
        }
    }

    xv->size = 0;
}