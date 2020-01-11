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

    if (!new_data) return -1;

    xs->data = new_data;
    xs->capacity = new_cap;
    return 0;
}

xstr_t* xstr_new(size_t capacity)
{
    xstr_t* r = malloc(sizeof(xstr_t));

    if (r)
    {
        r->size = 0;
        r->capacity = capacity > 0 ? capacity : XSTR_DEFAULT_CAPACITY;
        r->data = malloc(r->capacity);

        if (!r->data)
        {
            free(r);
            return NULL;
        }

        r->data[0] = '\0';
    }

    return r;
}

xstr_t* xstr_new_with(const char* cstr, int size)
{
    xstr_t* r;

    if (size < 0)
        for (size = 0; cstr[size]; ++size) { }

    r = xstr_new(size + 1);

    if (r)
    {
        memcpy(r->data, cstr, size);
        r->data[size] = '\0';
        r->size = size;
    }

    return r;
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

void xstr_append(xstr_t* xs, const char* cstr, int size)
{
    if (size < 0)
        for (size = 0; cstr[size]; ++size) { }

    if (need_expand(xs, size)
        && capacity_expand(xs, size) != 0)
        return; /* ignore expand failed */

    memcpy(xs->data + xs->size, cstr, size);

    xs->size += size;
    xs->data[xs->size] = '\0';
}

void xstr_append_at(xstr_t* xs, size_t pos, const char* cstr, int size)
{
    xs->size = pos;
    xstr_append(xs, cstr, size);
}

void xstr_insert(xstr_t* xs, size_t pos, const char* cstr, int size)
{
    if (size < 0)
        for (size = 0; cstr[size]; ++size) { }

    if (need_expand(xs, size)
        && capacity_expand(xs, size) != 0)
        return; /* ignore expand failed */

    /* no boundary check */
    memmove(xs->data + pos + size, xs->data + pos,
                xs->size - pos); /* >>> */
    memcpy(xs->data + pos, cstr, size);

    xs->size += size;
    xs->data[xs->size] = '\0';
}

void xstr_erase(xstr_t* xs, size_t pos, int count)
{
    /* no boundary check */
    if (count < 0)
    {
        xs->size = pos;
        xs->data[pos] = '\0';
    }
    else
    {
        memmove(xs->data + pos, xs->data + pos + count,
                    xs->size - pos - count); /* <<< */
        xs->size -= count;
        xs->data[xs->size] = '\0';
    }
}

void xstr_push_back(xstr_t* xs, char ch)
{
    if(need_expand(xs, 1)
        && capacity_expand(xs, 1) != 0)
        return; /* ignore expand failed */

    xs->data[xs->size++] = ch;
    xs->data[xs->size] = '\0';
}

void xstr_pop_back(xstr_t* xs)
{
    xs->data[--xs->size] = '\0';
}

const char g_xstr_i2c_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

char* uitoa(char* buf, unsigned int val, int radix)
{
    int off = 0;
    unsigned int v = val;

    if (!v) { ++off; }
    else do
    {
        v /= radix;
        ++off;
    } while (v);

    buf[off] = '\0';
    while (off)
    {
        buf[--off] = g_xstr_i2c_table[val % radix];
        val /= radix;
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

unsigned int atoui(const char* str, int base)
{
    unsigned int acc = 0;
    unsigned char v;

    while (*str)
    {
        v = g_xstr_c2i_table
                [(unsigned char)*str++];

        if (v > base) break;

        acc = acc * base + v;
    }

    return acc;
}
