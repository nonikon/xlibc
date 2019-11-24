#include <stdlib.h>
#include <string.h>

#include "xhash.h"

static int buckets_expand(xhash_t* xh)
{
    size_t new_sz = xh->bkt_size << 1;
    xhash_node_t** new_bkts = realloc(xh->buckets,
                        sizeof(xhash_node_t*) * new_sz);

    if (!new_bkts) return -1;

    size_t i;
    xhash_iter_t unlinked;
    xhash_iter_t iter;

    /* rehash */
    for (i = 0; i < xh->bkt_size; ++i)
    {
        iter = new_bkts[i];

        while (iter)
        {
            if (iter->hash & xh->bkt_size)
            {
                /* unlink this node */
                unlinked = iter;
                iter = unlinked->next;

                if (unlinked->next)
                    unlinked->next->prev = unlinked->prev;
                if (unlinked->prev)
                    unlinked->prev->next = iter;
                else
                    new_bkts[i] = iter;

                /* insert unlinked node into buckets[i + bkt_size] */
                unlinked->prev = NULL;
                unlinked->next = new_bkts[i + xh->bkt_size];

                if (unlinked->next)
                    unlinked->next->prev = unlinked;

                new_bkts[i + xh->bkt_size] = unlinked;
            }
            else
            {
                iter = iter->next;
            }
        }
    }

    xh->bkt_size = new_sz;
    xh->buckets = new_bkts;
    return 0;
}

xhash_t* xhash_new(int size, size_t data_size, xhash_hash_cb hash_cb,
            xhash_equal_cb equal_cb, xhash_destroy_cb destroy_cb)
{
    xhash_t* xh = malloc(sizeof(xhash_t));

    if (xh)
    {
        xh->hash_cb = hash_cb;
        xh->equal_cb = equal_cb;
        xh->destroy_cb = destroy_cb;

        xh->bkt_size = size > 0 ? size : XHASH_DEFAULT_SIZE;
        xh->data_size = data_size;
        xh->size = 0;
        xh->loadfactor = XHASH_DEFAULT_LOADFACTOR;

        xh->buckets = malloc(sizeof(xhash_node_t*) * xh->bkt_size);

        if (!xh->buckets)
        {
            free(xh);
            return NULL;
        }

        memset(xh->buckets, 0, sizeof(xhash_node_t*) * xh->bkt_size);
    }

    return xh;
}

void xhash_free(xhash_t* xh)
{
    if (xh)
    {
        xhash_clear(xh);
        free(xh->buckets);
        free(xh);
    }
}

xhash_iter_t xhash_put(xhash_t* xh, const void* pdata)
{
    unsigned hash = xh->hash_cb((void*)pdata);
    xhash_iter_t* iter = &xh->buckets[hash & (xh->bkt_size - 1)];
    xhash_node_t* prev = NULL;

    while (*iter)
    {
        if (hash == (*iter)->hash
            && xh->equal_cb(xhash_iter_data(*iter), (void*)pdata))
        {
            return *iter;
        }

        prev = *iter;
        iter = &(*iter)->next;
    }

    *iter = malloc(sizeof(xhash_node_t) + xh->data_size);

    if (*iter)
    {
        memcpy(xhash_iter_data(*iter), pdata, xh->data_size);

        (*iter)->prev = prev;
        (*iter)->next = NULL;
        (*iter)->hash = hash;

        ++xh->size;

        /* check loadfactor */
        if (xh->size * 100 / xh->bkt_size > xh->loadfactor)
            buckets_expand(xh);
    }

    return *iter;
}

xhash_iter_t xhash_get(xhash_t* xh, const void* pdata)
{
    unsigned hash = xh->hash_cb((void*)pdata);
    xhash_iter_t iter = xh->buckets[hash & (xh->bkt_size - 1)];

    while (iter)
    {
        if (hash == iter->hash
            && xh->equal_cb(xhash_iter_data(iter), (void*)pdata))
        {
            return iter;
        }

        iter = iter->next;
    }

    return NULL;
}

void xhash_remove(xhash_t* xh, xhash_iter_t iter)
{
    if (iter->prev)
        iter->prev->next = iter->next;
    else
        xh->buckets[iter->hash & (xh->bkt_size - 1)] = iter->next;

    if (iter->next)
        iter->next->prev = iter->prev;

    if (xh->destroy_cb)
        xh->destroy_cb(xhash_iter_data(iter));
    free(iter);

    --xh->size;
}

void xhash_clear(xhash_t* xh)
{
    size_t i;
    xhash_node_t* curr = NULL;
    xhash_node_t* next;

    for (i = 0; i < xh->bkt_size; ++i)
    {
        curr = xh->buckets[i];

        while (curr)
        {
            next = curr->next;

            if (xh->destroy_cb)
                xh->destroy_cb(xhash_iter_data(curr));
            free(curr);

            curr = next;
        }

        xh->buckets[i] = NULL;
    }

    xh->size = 0;
}

xhash_iter_t xhash_begin(xhash_t* xh)
{
    for (size_t i = 0; i < xh->bkt_size; ++i)
    {
        if (xh->buckets[i])
            return xh->buckets[i];
    }

    return NULL;
}

xhash_iter_t xhash_iter_next(xhash_t* xh, xhash_iter_t iter)
{
    if (iter->next) return iter->next;

    size_t i = iter->hash & (xh->bkt_size - 1);

    while (++i < xh->bkt_size)
    {
        if (xh->buckets[i])
            return xh->buckets[i];
    }

    return NULL;
}