#include <stdlib.h>
#include <string.h>

#include "xlist.h"

xlist_t* xlist_new(size_t val_size, xlist_destroy_cb cb)
{
    xlist_t* r = malloc(sizeof(xlist_t));

    if (r)
    {
        r->size = 0;
        r->val_size = val_size;
        r->destroy_cb = cb;
#ifndef XLIST_NO_CACHE
        r->cache = NULL;
#endif
        r->head.next = &r->head;
        r->head.prev = &r->head;
    }

    return r;
}

void xlist_free(xlist_t* xl)
{
    if (xl)
    {
        xlist_clear(xl);
#ifndef XLIST_NO_CACHE
        xlist_cache_free(xl);
#endif
        free(xl);
    }
}

void xlist_clear(xlist_t* xl)
{
    xlist_iter_t iter = xlist_begin(xl);

    while (xlist_iter_valid(xl, iter))
    {
        iter = iter->next;
        if (xl->destroy_cb)
            xl->destroy_cb(xlist_iter_value(iter->prev));
        free(iter->prev);
    }

    xl->size = 0;
    xl->head.prev = iter;
    xl->head.next = iter;
}

xlist_iter_t xlist_insert(xlist_t* xl, xlist_iter_t iter, const void* pvalue)
{
    xlist_iter_t newi;

#ifndef XLIST_NO_CACHE
    if (xl->cache)
    {
        newi = xl->cache;
        xl->cache = xl->cache->next;
    }
    else
    {
#endif
        newi = malloc(sizeof(xlist_node_t) + xl->val_size);
        if (!newi)
            return NULL;
#ifndef XLIST_NO_CACHE
    }
#endif

    newi->next = iter;
    newi->prev = iter->prev;
    iter->prev->next = newi;
    iter->prev = newi;

    ++xl->size;
    if (pvalue)
        memcpy(xlist_iter_value(newi), pvalue, (xl)->val_size);

    return newi;
}

xlist_iter_t xlist_erase(xlist_t* xl, xlist_iter_t iter)
{
    xlist_iter_t r = iter->next;

    --xl->size;
    iter->prev->next = iter->next;
    iter->next->prev = iter->prev;

    if (xl->destroy_cb)
        xl->destroy_cb(xlist_iter_value(iter));

#ifndef XLIST_NO_CACHE
    iter->next = xl->cache;
    xl->cache = iter;
#else
    free(iter);
#endif

    return r;
}

#ifndef XLIST_NO_CACHE
void xlist_cache_free(xlist_t* xl)
{
    xlist_node_t* c = xl->cache;

    while (c)
    {
        xl->cache = c->next;
        free(c);
        c = xl->cache;
    }
}
#endif

void* xlist_cut(xlist_t* xl, xlist_iter_t iter)
{
    --xl->size;
    iter->prev->next = iter->next;
    iter->next->prev = iter->prev;

    return xlist_iter_value(iter);
}

xlist_iter_t xlist_paste(xlist_t* xl, xlist_iter_t iter, void* pvalue)
{
    xlist_iter_t newi = xlist_value_iter(pvalue);

    ++xl->size;
    newi->next = iter;
    newi->prev = iter->prev;
    iter->prev->next = newi;
    iter->prev = newi;

    return newi;
}

void xlist_cut_free(xlist_t* xl, void* pvalue)
{
#ifndef XLIST_NO_CACHE
    xlist_iter_t iter = xlist_value_iter(pvalue);

    if (xl->destroy_cb)
        xl->destroy_cb(pvalue);

    iter->next = xl->cache;
    xl->cache = iter;
#else
    if (xl->destroy_cb)
        xl->destroy_cb(pvalue);
    free(xlist_value_iter(pvalue));
#endif
}