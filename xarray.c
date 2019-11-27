#include <stdlib.h>
#include <string.h>

#include "xarray.h"

xarray_t* xarray_new(size_t val_size, xarray_destroy_cb cb)
{
    xarray_t* array = malloc(sizeof(xarray_t));

    if (array)
    {
        memset(array, 0, sizeof(xarray_t));

        array->val_size = val_size;
        array->destroy_cb  = cb;
        /* sizeof(xuint) * 8 - XARRAY_BITS */
        array->root.shift  = (sizeof(xuint) << 3) - XARRAY_BITS;
    }

    return array;
}

void xarray_free(xarray_t* array)
{
    if (array)
    {
        xarray_clear(array);
        free(array);
    }
}

void* xarray_set(xarray_t* array, xuint index, void* pvalue)
{
    xarray_block_t* parent = &array->root;
    xarray_block_t* child;
    int i;

    while (parent->shift > 0)
    {
        i = (index >> parent->shift) & XARRAY_MASK;
        child = parent->values[i];

        if (!child)
        {
            child = malloc(sizeof(xarray_block_t));

            if (!child) return NULL;

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

    i = index & XARRAY_MASK;

    xarray_node_t* node = parent->values[i];

    if (!node)
    {
        node = malloc(sizeof(xarray_node_t) + array->val_size);

        if (!node) return NULL;

        node->block = parent;
        node->index = index;

        parent->values[i] = node;

        ++array->values;
        ++parent->used;
    }
    else if (array->destroy_cb)
    {
        // index has already been set, destroy it
        array->destroy_cb(xarray_iter_value(node));
    }

    if (pvalue)
        memcpy(xarray_iter_value(node), pvalue, array->val_size);

    return xarray_iter_value(node);
}

void xarray_unset(xarray_t* array, xuint index)
{
    xarray_block_t* block = &array->root;
    int i;

    while (block->shift > 0)
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return; /* has not been set */
    }

    i = index & XARRAY_MASK;

    if (block->values[i])
    {
        /* destroy a node */
        if (array->destroy_cb)
            array->destroy_cb(xarray_iter_value(
                                (xarray_iter_t)block->values[i]));
        free(block->values[i]);

        --array->values;
        --block->used;
        
        /* release chain if 'used == 0' */
        while (!block->used)
        {
            i = block->parent_pos;
            block = block->parent_block;

            /* destroy a block */
            free(block->values[i]);

            --array->blocks;
            --block->used;
        }

        block->values[i] = NULL;
    }
}

void* xarray_get(xarray_t* array, xuint index)
{
    xarray_block_t* block = &array->root;
    int i;

    while (block->shift > 0)
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return NULL;
    }

    return xarray_iter_value(
        (xarray_iter_t)block->values[index & XARRAY_MASK]);
}

void xarray_clear(xarray_t* array)
{
    xarray_block_t* block = &array->root;
    int i = 0;

    do
    {
        if (i < XARRAY_BLOCK_SIZE)
        {
            if (block->values[i])
            {
                if (block->shift != 0) /* is a block, step in it */
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
                ++i;
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
    array->root.shift = (sizeof(xuint) << 3) - XARRAY_BITS;
}

xarray_iter_t xarray_begin(xarray_t* array)
{
    xarray_block_t* block = &array->root;
    int i = 0;

    while (i < XARRAY_BLOCK_SIZE)
    {
        if (block->values[i])
        {
            if (block->shift != 0) /* is a block */
            {
                block = block->values[i];
                i = 0;
            }
            else /* is a node */
                return block->values[i];
        }
        else
            ++i;
    }

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
            if (block->values[i])
            {
                if (block->shift != 0)
                {
                    block = block->values[i];
                    i = 0;
                }
                else
                    return block->values[i];
            }
            else
                ++i;
        }
        else
        {
            /* goto parent block */
            i = block->parent_pos + 1;
            block = block->parent_block;

            if (!block) return NULL;
        }
    }
    while (1);

    return NULL;
}
