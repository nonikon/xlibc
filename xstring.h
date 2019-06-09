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

typedef struct xstr xstr_t;

/* allocate memory for a 'xstr_t' with initial capacity. */
xstr_t* xstr_new(size_t capacity);
/* allocate memory for a 'xstr_t' and initialize with 'cstr'. 'size' can be <= 0. */
xstr_t* xstr_new_with(const char* cstr, int size);
/* release memory for a 'xstr_t'. */
void xstr_free(xstr_t* xs);

/* returns a pointer to the first character of a string. */
xchar* xstr_data(xstr_t* xs);
/* accesses the first character. */
xchar xstr_front(const xstr_t* xs);
/* accesses the last character, 'xs->size' must > 0. */
xchar xstr_back(const xstr_t* xs);
/* checks whether the string is empty. */
int xstr_empty(const xstr_t* xs);
/* returns the number of characters. */
size_t xstr_size(const xstr_t* xs);

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
 * string -> unsigned int.
 * base: 2 ~ 36.
 */
unsigned int atoui(const char* str, int slen, int base);

#endif // _XSTRING_H_