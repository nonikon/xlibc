#include <stdlib.h>
#include <string.h>
#include "xarray.h"

xarray_t* xarray_new(unsigned int val_size, xarray_destroy_cb cb)
{
    xarray_t* array = malloc(sizeof(xarray_t));

    if (array)
    {
        memset(array, 0, sizeof(xarray_t));

        array->val_size    = val_size;
        array->destroy_cb  = cb;
        array->root.depth  = 1;
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
    xarray_block_t* block = &array->root;
    unsigned int i;
    void* p;

    while (block->depth < XARRAY_MAX_DEPTH)
    {
        i = (index >> XARRAY_BITS * (XARRAY_MAX_DEPTH - block->depth)) & XARRAY_MASK;
        p = block->values[i];

        if (!p)
        {
            p = malloc(sizeof(xarray_block_t));
            if (!p)
                return NULL;

            memset(p, 0, sizeof(xarray_block_t));

            ((xarray_block_t*)p)->parent.block = block;
            ((xarray_block_t*)p)->parent.index = (block->parent.index << XARRAY_BITS) | i;
            ((xarray_block_t*)p)->parent.pos   = i;
            ((xarray_block_t*)p)->depth = block->depth + 1;

            ++array->blocks;
            ++block->used;
            block->values[i] = p;
        }

        block = p;
    }

    i = index & XARRAY_MASK;
    p = block->values[i];

    if (!p)
    {
        p = malloc(array->val_size);
        if (!p)
            return NULL;

        ++array->values;
        ++block->used;
        block->values[i] = p;
    }

    if (pvalue)
        memcpy(p, pvalue, array->val_size);

    return p;
}

void xarray_reset(xarray_t* array, unsigned int index)
{
    xarray_block_t* block = &array->root;
    unsigned int i;

    while (block->depth < XARRAY_MAX_DEPTH)
    {
        i = (index >> XARRAY_BITS * (XARRAY_MAX_DEPTH - block->depth)) & XARRAY_MASK;

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
        free(block->values[i]);

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

    while (block->depth < XARRAY_MAX_DEPTH)
    {
        i = (index >> XARRAY_BITS * (XARRAY_MAX_DEPTH - block->depth)) & XARRAY_MASK;

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
                if (block->depth == XARRAY_MAX_DEPTH)
                {
                    if (array->destroy_cb)
                        array->destroy_cb(block->values[i]);
                    free(block->values[i++]);
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
    array->root.depth = 1;
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
                if (block->depth == XARRAY_MAX_DEPTH)
                    break;
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

    iter->block = block;
    iter->index = block ? (block->parent.index << XARRAY_BITS) | i : 0;
    iter->pos   = i;
}

inline int xarray_iter_valid(xarray_iter_t* iter)
{
    return iter->block != NULL;
}

inline unsigned int xarray_iter_index(xarray_iter_t* iter)
{
    return iter->index;
}

inline void* xarray_iter_value(xarray_iter_t* iter)
{
    return iter->block->values[iter->pos];
}
