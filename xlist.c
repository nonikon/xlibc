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
        free(xl);
    }
}

void xlist_clear(xlist_t* xl)
{
    xlist_iter_t iter = xlist_begin(xl);

    if (xl->destroy_cb) while (xlist_iter_valid(xl, iter))
    {
        iter = iter->next;
        xl->destroy_cb(xlist_iter_value(iter->prev));
        free(iter->prev);
    }
    else while (xlist_iter_valid(xl, iter))
    {
        iter = iter->next;
        free(iter->prev);
    }

    xl->size = 0;
    xl->head.prev = iter;
    xl->head.next = iter;
}

xlist_iter_t xlist_insert(xlist_t* xl, xlist_iter_t iter, const void* pvalue)
{
    xlist_iter_t newi = malloc(sizeof(xlist_node_t) + xl->val_size);

    if (newi)
    {
        newi->next = iter;
        newi->prev = iter->prev;
        iter->prev->next = newi;
        iter->prev = newi;

        ++xl->size;
        if (pvalue)
        {
            xlist_set_value(xl, newi, pvalue);
        }
    }

    return newi;
}

xlist_iter_t xlist_erase(xlist_t* xl, xlist_iter_t iter)
{
    xlist_iter_t r = iter->next;

    --xl->size;
    iter->prev->next = iter->next;
    iter->next->prev = iter->prev;

    if (xl->destroy_cb)
    {
        xl->destroy_cb(xlist_iter_value(iter));
    }
    free(iter);

    return r;
}

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
    if (xl->destroy_cb)
    {
        xl->destroy_cb(pvalue);
    }

    free(xlist_value_iter(pvalue));
}