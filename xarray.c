#include <stdlib.h>
#include <string.h>

#include "xarray.h"

#define XARRAY_MASK         (XARRAY_BLOCK_SIZE - 1)
/* sizeof(xuint) * 8 - XARRAY_BITS */
#define XARRAY_MAX_SHIFT    ((sizeof(xuint) << 3) - XARRAY_BITS)

xarray_t* xarray_new(size_t val_size, xarray_destroy_cb cb)
{
    xarray_t* array = malloc(sizeof(xarray_t));

    if (array)
    {
        memset(array, 0, sizeof(xarray_t));

        array->val_size = val_size;
        array->destroy_cb = cb;
        array->root.shift = XARRAY_MAX_SHIFT;
    }

    return array;
}

void xarray_free(xarray_t* array)
{
    if (array)
    {
        xarray_clear(array);
#ifndef XARRAY_NO_CACHE
        xarray_node_cache_free(array);
        xarray_block_cache_free(array);
#endif
        free(array);
    }
}

xarray_iter_t xarray_set(xarray_t* array, xuint index, const void* pvalue)
{
    xarray_block_t* parent = &array->root;
    xarray_block_t* child;
    int i;

    do
    {
        i = (index >> parent->shift) & XARRAY_MASK;
        child = parent->values[i];

        if (!child)
        {
#ifndef XARRAY_NO_CACHE
            if (array->blk_cache)
            {
                child = array->blk_cache;
                array->blk_cache = child->parent_block;
            }
            else
            {
#endif
                child = malloc(sizeof(xarray_block_t));
                if (!child)
                    return NULL;
#ifndef XARRAY_NO_CACHE
            }
#endif
            memset(child, 0, sizeof(xarray_block_t));

            child->parent_block = parent;
            child->parent_pos = i;
            child->shift = parent->shift - XARRAY_BITS;

            ++array->blocks;
            ++parent->used;

            parent->values[i] = child;
        }

        parent = child;
    }
    while (parent->shift > 0);

    i = index & XARRAY_MASK;

    xarray_node_t* node = parent->values[i];

    if (!node)
    {
#ifndef XARRAY_NO_CACHE
        if (array->nod_cache)
        {
            node = array->nod_cache;
            array->nod_cache = (xarray_node_t*)node->block;
        }
        else
        {
#endif
            node = malloc(sizeof(xarray_node_t) + array->val_size);
            if (!node)
                return NULL;
#ifndef XARRAY_NO_CACHE
        }
#endif
        node->block = parent;
        node->index = index;

        parent->values[i] = node;

        ++array->values;
        ++parent->used;
    }
    else if (array->destroy_cb)
    {
        /* index has already been set, destroy it */
        array->destroy_cb(xarray_iter_value(node));
    }

    if (pvalue)
        memcpy(xarray_iter_value(node), pvalue, array->val_size);

    return node;
}

void xarray_unset(xarray_t* array, xuint index)
{
    xarray_block_t* block = &array->root;
    int i;

    do
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return; /* has not been set */
    }
    while (block->shift > 0);

    i = index & XARRAY_MASK;

    if (block->values[i])
    {
        /* destroy a node */
        if (array->destroy_cb)
            array->destroy_cb(xarray_iter_value(
                        (xarray_iter_t)block->values[i]));
#ifndef XARRAY_NO_CACHE
        /* use 'xarray_node_t.block' to store next cache node */
        ((xarray_node_t*)block->values[i])->block
                        = (xarray_block_t*)array->nod_cache;
        array->nod_cache = block->values[i];
#else
        free(block->values[i]);
#endif
        --array->values;
        --block->used;

        /* release block chain if 'used == 0' */
        while (!block->used)
        {
            i = block->parent_pos;
            block = block->parent_block;

            /* destroy a block */
#ifndef XARRAY_NO_CACHE
            /* use 'xarray_block_t.parent_block' to store next cache block */
            ((xarray_block_t*)block->values[i])
                        ->parent_block = array->blk_cache;
            array->blk_cache = block->values[i];
#else
            free(block->values[i]);
#endif
            --array->blocks;
            --block->used;
        }

        block->values[i] = NULL;
    }
}

xarray_iter_t xarray_get(xarray_t* array, xuint index)
{
    xarray_block_t* block = &array->root;
    int i;

    do
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return NULL;
    }
    while (block->shift > 0);

    return block->values[index & XARRAY_MASK];
}

void xarray_clear(xarray_t* array)
{
    xarray_block_t* block = &array->root;
    int i = 0;

    do
    {
        if (i < XARRAY_BLOCK_SIZE)
        {
            if (!block->values[i])
                ++i;
            else if (block->shift != 0) /* is a block, step in it */
            {
                block = block->values[i];
                i = 0;
            }
            else /* is a node, destroy it */
            {
                if (array->destroy_cb)
                    array->destroy_cb(xarray_iter_value(
                                (xarray_iter_t)block->values[i]));
                free(block->values[i]);

                --array->values;
                ++i;
            }
        }
        else
        {
            /* goto parent block */
            i = block->parent_pos;
            block = block->parent_block;

            if (!block) break;

            /* destroy child block */
            free(block->values[i++]);

            --array->blocks;
        }
    }
    while (1);

    memset(&array->root, 0, sizeof(xarray_block_t));
    array->root.shift = XARRAY_MAX_SHIFT;
}

#ifndef XARRAY_NO_CACHE
void xarray_node_cache_free(xarray_t* array)
{
    xarray_node_t* c = array->nod_cache;

    while (c)
    {
        array->nod_cache = (xarray_node_t*)c->block;
        free(c);
        c = array->nod_cache;
    }
}

void xarray_block_cache_free(xarray_t* array)
{
    xarray_block_t* c = array->blk_cache;

    while (c)
    {
        array->blk_cache = c->parent_block;
        free(c);
        c = array->blk_cache;
    }
}
#endif

xarray_iter_t xarray_begin(xarray_t* array)
{
    xarray_block_t* block = &array->root;
    int i = 0;

    do
    {
        if (!block->values[i])
            ++i;
        else if (block->shift != 0) /* is a block */
        {
            block = block->values[i];
            i = 0;
        }
        else /* is a node */
        {
            return block->values[i];
        }
    }
    while (i < XARRAY_BLOCK_SIZE);

    return NULL;
}

xarray_iter_t xarray_iter_next(xarray_iter_t iter)
{
    xarray_block_t* block = iter->block;
    int i = (iter->index & XARRAY_MASK) + 1;

    do
    {
        if (i < XARRAY_BLOCK_SIZE)
        {
            if (!block->values[i])
                ++i;
            else if (block->shift != 0) /* is a block */
            {
                block = block->values[i];
                i = 0;
            }
            else /* is a node */
            {
                return block->values[i];
            }
        }
        else
        {
            /* goto parent block */
            i = block->parent_pos + 1;
            block = block->parent_block;

            if (!block)
                return NULL;
        }
    }
    while (1);

    return NULL;
}
