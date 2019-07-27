#ifndef _XSTRING_H_
#define _XSTRING_H_

#include <stddef.h>

/* similar to C++ std::string libray. */

#ifndef XSTR_DEFAULT_CAPACITY
#define XSTR_DEFAULT_CAPACITY    36
#endif

// #ifdef UNICODE
// typedef wchar_t xchar;
// #else
typedef char xchar;
// #endif

typedef struct xstr
{
    xchar* data;
    size_t size;
    size_t capacity;
} xstr_t;

/* allocate memory for a string with initial capacity. */
xstr_t* xstr_new(size_t capacity);
/* allocate memory for a string and initialize with 'cstr'. 'size' can be <= 0. */
xstr_t* xstr_new_with(const char* cstr, int size);
/* release memory for a string. */
void xstr_free(xstr_t* xs);

/* allocate memory for a string and initialize with 'xs'. */
#define xstr_new_with_str(xs) \
    xstr_new_with(xstr_data(xs), xstr_size(xs))

/* returns a pointer to the first character of a string. */
#define xstr_data(xs)   ((xs)->data)
/* accesses the first character. */
#define xstr_front(xs)  ((xs)->data[0])
/* accesses the last character, 'xs->size' must > 0. */
#define xstr_back(xs)   ((xs)->data[(xs)->size - 1])
/* checks whether the string is empty. */
#define xstr_empty(xs)  ((xs)->size == 0)
/* returns the number of characters. */
#define xstr_size(xs)   ((xs)->size)

/* clears the contents. */
void xstr_clear(xstr_t* xs);
/* appends characters to the end, 'size' < 0 means auto calculate. */
void xstr_append(xstr_t* xs, const xchar* cstr, int size);
/* appends characters starting at 'pos', 'size' can be negative. */
void xstr_append_at(xstr_t* xs, int pos, const xchar* cstr, int size);
/* assign characters to a string. */
void xstr_assign(xstr_t* xs, const xchar* cstr, int size);
/* appends a character to the end. */
void xstr_push_back(xstr_t* xs, xchar ch);
/* remove and return the last character, 'xs->size' must > 0. */
xchar xstr_pop_back(xstr_t* xs);
/* removes 'count' characters starting at 'pos', 'count' == -1 means end of 'xs'. */
void xstr_erase(xstr_t* xs, int pos, int count);

/* append a 'xstr' to the end. */
#define xstr_append_str(dest, src) \
    xstr_append(dest, xstr_data(src), xstr_size(src))
/* append a 'xstr' starting at 'pos'. */
#define xstr_append_str_at(dest, pos, src) \
    xstr_append_at(dest, pos, xstr_data(src), xstr_size(src))
/* assign 'xstr' to a string. */
#define xstr_assgin_str(dest, src) \
    xstr_assgin(dest, xstr_data(src), xstr_size(src))

/*
 * unsigned int -> string, return a pointer pointed to 'buf'.
 * 'buf' size 36 may be the best.
 * radix: 2 ~ 36.
 * ex:
 *     val = 65535, radix = 10 -> buf = "65535".
 *     val = 65535, radix = 16 -> buf = "FFFF".
 */
char* uitoa(char* buf, unsigned int val, int radix);
/*
 * unsigned char -> hex string, return a pointer pointed to 'buf'.
 */
char* uctoa_hex(char buf[2], unsigned char val);
/*
 * string -> unsigned int.
 * base: 2 ~ 36.
 */
unsigned int atoui(const char* str, int slen, int base);
/*
 * hex string -> unsigned char.
 */
unsigned char atouc_hex(const char str[2]);

#endif // _XSTRING_H_