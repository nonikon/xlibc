/*
 * Copyright (C) 2019-2021 nonikon@qq.com.
 * All rights reserved.
 */

#include <stdlib.h>
#include <string.h>

#include "xstring.h"

#define need_expand(xs, sz) \
            (xs)->size + sz + 1 > (xs)->capacity /* leave 1 null-terminated */

static int capacity_expand(xstr_t* xs, size_t size)
{
    size_t new_cap = xs->capacity << 1; /* xs->capacity * 2; */
    void* new_data;

    size += xs->size + 1; /* leave 1 null-terminated */

    if (new_cap < size) new_cap = size;

    new_data = realloc(xs->data, new_cap);

    if (new_data)
    {
        xs->data = new_data;
        xs->capacity = new_cap;
        return 0;
    }

    return -1;
}

xstr_t* xstr_init(xstr_t* xs, int capacity)
{
    xs->capacity = capacity > 0
            ? capacity : XSTR_DEFAULT_CAPACITY;
    xs->data = malloc(xs->capacity);

    if (xs->data)
    {
        xs->size = 0;
        xs->data[0] = '\0';
        return xs;
    }

    return NULL;
}

xstr_t* xstr_init_with(xstr_t* xs, const char* cstr, int size)
{
    if (size < 0) size = strlen(cstr);

    xs->capacity = size + 1;
    xs->data = malloc(xs->capacity);

    if (xs->data)
    {
        memcpy(xs->data, cstr, size);

        xs->size = size;
        xs->data[size] = '\0';
        return xs;
    }

    return NULL;
}

void xstr_destroy(xstr_t* xs)
{
    free(xs->data);
}

xstr_t* xstr_new(int capacity)
{
    xstr_t* r = malloc(sizeof(xstr_t));

    if (r)
    {
        if (xstr_init(r, capacity))
            return r;
        free(r);
    }

    return NULL;
}

xstr_t* xstr_new_with(const char* cstr, int size)
{
    xstr_t* r = malloc(sizeof(xstr_t));

    if (r)
    {
        if (xstr_init_with(r, cstr, size))
            return r;
        free(r);
    }

    return NULL;
}

void xstr_free(xstr_t* xs)
{
    if (xs)
    {
        free(xs->data);
        free(xs);
    }
}

void xstr_clear(xstr_t* xs)
{
    xs->size = 0;
    xs->data[0] = '\0';
}

void xstr_assign_at(xstr_t* xs, size_t pos, const char* cstr, int size)
{
    xs->size = pos;
    xstr_append(xs, cstr, size);
}

void xstr_append(xstr_t* xs, const char* cstr, int size)
{
    if (size < 0) size = strlen(cstr);

    if (need_expand(xs, size))
        capacity_expand(xs, size); /* ignore expand failed */

    memcpy(xs->data + xs->size, cstr, size);

    xs->size += size;
    xs->data[xs->size] = '\0';
}

void xstr_insert(xstr_t* xs, size_t pos, const char* cstr, int size)
{
    if (size < 0) size = strlen(cstr);

    if (need_expand(xs, size))
        capacity_expand(xs, size); /* ignore expand failed */

    /* >>> */
    memmove(xs->data + pos + size,
            xs->data + pos, xs->size - pos);

    memcpy(xs->data + pos, cstr, size);

    xs->size += size;
    xs->data[xs->size] = '\0';
}

void xstr_erase(xstr_t* xs, size_t pos, int count)
{
    if (count < 0 || pos + count >= xs->size)
    {
        xs->size = pos;
        xs->data[pos] = '\0';
    }
    else
    {
        xs->size -= count;
        /* <<< */
        memmove(xs->data + pos, xs->data + pos + count,
                xs->size - pos);
        xs->data[xs->size] = '\0';
    }
}

void xstr_push_back(xstr_t* xs, char ch)
{
    if(need_expand(xs, 1))
        capacity_expand(xs, 1); /* ignore expand failed */

    xs->data[xs->size++] = ch;
    xs->data[xs->size] = '\0';
}

void xstr_pop_back(xstr_t* xs)
{
    xs->data[--xs->size] = '\0';
}

#if XSTR_ENABLE_EXTRA
const char g_xstr_i2c_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

char* xultoa(char* buf, unsigned long val, unsigned radix)
{
    int l = 0;
    int r = 0;
    char c;

    if (!val)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    do
    {
        buf[r++] = g_xstr_i2c_table[val % radix];
        val /= radix;
    }
    while (val);

    buf[r--] = '\0';

    /* string reverse */
    while (l < r)
    {
        c = buf[l];
        buf[l++] = buf[r];
        buf[r--] = c;
    }

    return buf;
}

const unsigned char g_xstr_c2i_table[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*0~15*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*16~31*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*32~47*/
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, /*48~63*/
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, /*64~79*/
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, /*80~95*/
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, /*64~79*/
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, /*112~127*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

unsigned long xatoul(const char* str, char** ep, unsigned base)
{
    unsigned long acc = 0;
    unsigned v;

    while (*str)
    {
        v = g_xstr_c2i_table[(unsigned char)*str];

        if (v >= base) break;

        acc = acc * base + v;
        str = str + 1;
    }

    if (ep) *ep = (char*)str;

    return acc;
}
#endif // XSTR_ENABLE_EXTRA
