/*
 * Copyright (C) 2019-2021 nonikon@qq.com.
 * All rights reserved.
 */

#ifndef _XSTRING_H_
#define _XSTRING_H_

#include <stddef.h>

/* similar to C++ std::string library. */

#ifdef HAVE_XCONFIG_H
#include "xconfig.h"
#else

/* enable xultoa/xatoul/... interface or not. */
#ifndef XSTR_ENABLE_EXTRA
#define XSTR_ENABLE_EXTRA       1
#endif

#ifndef XSTR_DEFAULT_CAPACITY
#define XSTR_DEFAULT_CAPACITY   32
#endif

#endif

typedef struct xstr xstr_t;

struct xstr
{
    char*   data;
    size_t  size;
    size_t  capacity;
};

/* initialize a 'xstr_t' with 'capacity'. 'capacity' <= 0 means default. */
xstr_t* xstr_init(xstr_t* xs, int capacity);
/* initialize a 'xstr_t' with 'cstr'. 'size' < 0 means 'strlen(cstr)'. */
xstr_t* xstr_init_with(xstr_t* xs, const char* cstr, int size);
/* destroy a 'xstr_t' which has called 'xstr_init*'. */
void xstr_destroy(xstr_t* xs);

/* allocate memory for a 'xstr_t' and initialize it with 'capacity'.
 * 'capacity' <= 0 means default. */
xstr_t* xstr_new(int capacity);
/* allocate memory for a 'xstr_t' and initialize it with 'cstr'.
 * 'size' < 0 means 'strlen(cstr)'. */
xstr_t* xstr_new_with(const char* cstr, int size);
/* release memory for a 'xstr_t' which 'xstr_new*' returns. */
void xstr_free(xstr_t* xs);

/* allocate memory for a 'xstr_t' and initialize it with 'xs'. */
#define xstr_new_with_str(xs) \
            xstr_new_with(xstr_data(xs), xstr_size(xs))

/* return a pointer to the first character. */
#define xstr_data(xs)       ((xs)->data)
/* accesse the first character. */
#define xstr_front(xs)      ((xs)->data[0])
/* accesse the last character, 'xs->size' must > 0. */
#define xstr_back(xs)       ((xs)->data[(xs)->size - 1])
/* check whether the 'xstr_t' is empty. */
#define xstr_empty(xs)      ((xs)->size == 0)
/* return the number of characters. */
#define xstr_size(xs)       ((xs)->size)
/* return the number of characters that can be held in currently allocated storage. */
#define xstr_capacity(xs)   ((xs)->capacity)

/* assign characters to a 'xstr_t' beginning at 'pos'.
 * 'pos' must <= 'xs->size'. 'size' < 0 means 'strlen(cstr)'. */
void xstr_assign_at(xstr_t* xs, size_t pos, const char* cstr, int size);
/* append characters to the end. 'size' < 0 means 'strlen(cstr)'. */
void xstr_append(xstr_t* xs, const char* cstr, int size);
/* insert characters at 'pos'. 'pos' must <= 'xs->size'.
 * 'size' < 0 means 'strlen(cstr)'. */
void xstr_insert(xstr_t* xs, size_t pos, const char* cstr, int size);
/* remove 'count' characters starting at 'pos'.
 * 'pos' must <= 'xs->size'. 'count' < 0 means end of 'xs'. */
void xstr_erase(xstr_t* xs, size_t pos, int count);
/* clear the contents. */
void xstr_clear(xstr_t* xs);

/* assign characters to a 'xstr_t'. */
#define xstr_assign(xs, cstr, sz)   xstr_assign_at(xs, 0, cstr, sz)
/* prepend characters to a 'xstr_t'. */
#define xstr_prepend(xs, cstr, sz)  xstr_insert(xs, 0, cstr, sz)

/* append a character to the end. */
void xstr_push_back(xstr_t* xs, char ch);
/* remove the last character. 'xs->size' must > 0. */
void xstr_pop_back(xstr_t* xs);

/* append 'src' (xstr_t) to 'dest' (xstr_t). */
#define xstr_append_str(dest, src) \
            xstr_append(dest, xstr_data(src), xstr_size(src))
/* prepend 'src' (xstr_t) to 'dest' (xstr_t). */
#define xstr_prepend_str(dest, src) \
            xstr_prepend(dest, xstr_data(src), xstr_size(src))
/* assign 'src' (xstr_t) to 'dest' (xstr_t). */
#define xstr_assign_str(dest, src) \
            xstr_assign(dest, xstr_data(src), xstr_size(src))
/* assign 'src' (xstr_t) to 'dest' (xstr_t) at 'pos'. */
#define xstr_assign_str_at(dest, pos, src) \
            xstr_assign_at(dest, pos, xstr_data(src), xstr_size(src))
/* insert 'src' (xstr_t) to 'dest' (xstr_t) at 'pos'. */
#define xstr_insert_str(dest, pos, src) \
            xstr_insert(dest, pos, xstr_data(src), xstr_size(src))

#if XSTR_ENABLE_EXTRA
extern const char g_xstr_i2c_table[];
extern const unsigned char g_xstr_c2i_table[];

/* unsigned long -> string, return a pointer pointed to 'buf'.
 * 'buf' size 36 may be the best.
 * radix: 2 ~ 36.
 * ex:
 *     val = 65535, radix = 10 -> buf = "65535".
 *     val = 65535, radix = 16 -> buf = "FFFF". */
char* xultoa(char* buf, unsigned long val, unsigned radix);
/* unsigned char -> hex string, return a pointer pointed to 'buf'. */
static inline void xuctoa_hex(char buf[2], unsigned char val)
{
    buf[0] = g_xstr_i2c_table[val >> 4];
    buf[1] = g_xstr_i2c_table[val & 15];
}

/* string -> unsigned long. similar to 'strtol'.
 * base: 2 ~ 36. */
unsigned long xatoul(const char* str, char** ep, unsigned base);
/* hex string -> unsigned char. */
static inline unsigned char xatouc_hex(const char str[2])
{
    return g_xstr_c2i_table[(unsigned char)str[1]]
            | (g_xstr_c2i_table[(unsigned char)str[0]] << 4);
}
#endif // XSTR_ENABLE_EXTRA

#endif // _XSTRING_H_
