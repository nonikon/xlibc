#include <stdlib.h>
#include <string.h>
#include "xarray.h"

xarray_t* xarray_new(xarray_destroy_cb cb)
{
    xarray_t* array = malloc(sizeof(xarray_t));

    if (array)
    {
        memset(array, 0, sizeof(xarray_t));

        array->destroy_cb  = cb;
        array->root.shift  = XARRAY_BITS * (XARRAY_MAX_DEPTH - 1);
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

void* xarray_set(xarray_t* array, unsigned int index, void* pvalue)
{
    xarray_block_t* parent = &array->root;
    xarray_block_t* child;
    unsigned int i;

    if (!pvalue) return NULL;

    while (parent->shift > 0)
    {
        i = (index >> parent->shift) & XARRAY_MASK;
        child = parent->values[i];

        if (!child)
        {
            child = malloc(sizeof(xarray_block_t));
            if (!child)
                return NULL;

            memset(child, 0, sizeof(xarray_block_t));

            child->parent.block = parent;
            child->parent.index = (parent->parent.index << XARRAY_BITS) | i;
            child->parent.pos   = i;
            child->shift = parent->shift - XARRAY_BITS;

            ++array->blocks;
            ++parent->used;

            parent->values[i] = child;
        }

        parent = child;
    }

    i = index & XARRAY_MASK;

    if (!parent->values[i])
        ++array->values, ++parent->used;
    else if (array->destroy_cb) // index has already been set, destroy it
        array->destroy_cb(parent->values[i]);

    parent->values[i] = pvalue;

    return pvalue;
}

void xarray_unset(xarray_t* array, unsigned int index)
{
    xarray_block_t* block = &array->root;
    unsigned int i;

    while (block->shift > 0)
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return;
    }

    i = index & XARRAY_MASK;

    if (block->values[i])
    {
        if (array->destroy_cb)
            array->destroy_cb(block->values[i]);

        --array->values;
        --block->used;
        // release chain if 'used == 0'.
        while (!block->used)
        {
            i = block->parent.pos;
            block = block->parent.block;

            --array->blocks;
            --block->used;
            free(block->values[i]);
        }

        block->values[i] = NULL;
    }
}

void* xarray_get(xarray_t* array, unsigned int index)
{
    xarray_block_t* block = &array->root;
    unsigned int i;

    while (block->shift > 0)
    {
        i = (index >> block->shift) & XARRAY_MASK;

        if (block->values[i])
            block = block->values[i];
        else
            return NULL;
    }

    return block->values[index & XARRAY_MASK];
}

void xarray_clear(xarray_t* array)
{
    xarray_block_t* block = &array->root;
    unsigned int i = 0;

    // printf("before %d %d, ", array->blocks, array->values);
    while (1)
    {
        if (i < XARRAY_BLOCK_SIZE)
        {
            if (block->values[i])
            {
                if (block->shift == 0)
                {
                    if (array->destroy_cb)
                        array->destroy_cb(block->values[i]);
                    ++i;
                    --array->values;
                }
                else
                {
                    block = block->values[i];
                    i = 0;
                }
            }
            else
                ++i;
        }
        else
        {
            i = block->parent.pos;
            block = block->parent.block;

            if (!block) break;

            free(block->values[i++]);
            --array->blocks;
        }
    }
    // printf("after %d %d.\n", array->blocks, array->values);

    memset(&array->root, 0, sizeof(xarray_block_t));
    array->root.shift = XARRAY_BITS * (XARRAY_MAX_DEPTH - 1);
}

void xarray_begin(xarray_t* array, xarray_iter_t* iter)
{
    iter->block = &array->root;
    iter->index = 0;
    iter->pos   = -1;

    xarray_iter_next(iter);
}

void xarray_iter_next(xarray_iter_t* iter)
{
    xarray_block_t* block = iter->block;
    unsigned int i = iter->pos + 1;

    while (block)
    {
        if (i < XARRAY_BLOCK_SIZE)
        {
            if (block->values[i])
            {
                if (block->shift == 0)
                {
                    iter->block = block;
                    iter->index = (block->parent.index << XARRAY_BITS) | i;
                    iter->pos   = i;
                    return;
                }
                block = block->values[i];
                i = 0;
            }
            else
                ++i;
        }
        else
        {
            i = block->parent.pos + 1;
            block = block->parent.block;
        }
    }

    iter->block = NULL;
}
