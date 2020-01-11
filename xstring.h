#ifndef _XSTRING_H_
#define _XSTRING_H_

#include <stddef.h>

/* similar to C++ std::string libray. */

#ifndef XSTR_DEFAULT_CAPACITY
#define XSTR_DEFAULT_CAPACITY    36
#endif

typedef struct xstr xstr_t;

struct xstr
{
    char*   data;
    size_t  size;
    size_t  capacity;
};

/* allocate memory for a string with initial capacity. */
xstr_t* xstr_new(size_t capacity);
/* allocate memory for a string and initialize with 'cstr'.
 * 'size' < 0 means auto calculate. */
xstr_t* xstr_new_with(const char* cstr, int size);
/* release memory for a string. */
void xstr_free(xstr_t* xs);

/* allocate memory for a string and initialize with 'xs'. */
#define xstr_new_with_str(xs) \
            xstr_new_with(xstr_data(xs), xstr_size(xs))

/* return a pointer to the first character of a string. */
#define xstr_data(xs)       ((xs)->data)
/* accesse the first character. */
#define xstr_front(xs)      ((xs)->data[0])
/* accesse the last character, 'xs->size' must > 0. */
#define xstr_back(xs)       ((xs)->data[(xs)->size - 1])
/* check whether the string is empty. */
#define xstr_empty(xs)      ((xs)->size == 0)
/* return the number of characters. */
#define xstr_size(xs)       ((xs)->size)
/* return the number of characters that can be held in currently allocated storage.  */
#define xstr_capacity(xs)   ((xs)->capacity)

/* clear the contents. */
void xstr_clear(xstr_t* xs);
/* append characters to the end. 'size' < 0 means auto calculate. */
void xstr_append(xstr_t* xs, const char* cstr, int size);
/* append characters starting at 'pos'. 'size' < 0 means auto calculate. */
void xstr_append_at(xstr_t* xs, size_t pos, const char* cstr, int size);
/* insert characters at 'pos'. 'size' < 0 means auto calculate. */
void xstr_insert(xstr_t* xs, size_t pos, const char* cstr, int size);
/* remove 'count' characters starting at 'pos', 'count' < 0 means end of 'xs'. */
void xstr_erase(xstr_t* xs, size_t pos, int count);

/* append a character to the end. */
void xstr_push_back(xstr_t* xs, char ch);
/* remove the last character. 'xs->size' must > 0. */
void xstr_pop_back(xstr_t* xs);

/* assign characters to a string. */
#define xstr_assign(xs, cstr, size) \
            xstr_append_at(xs, 0, cstr, size)
/* prepend characters to the begin. */
#define xstr_prepend(xs, cstr, size) \
            xstr_insert(xs, 0, cstr, size)
/* assign 'xstr' to a string. */
#define xstr_assign_str(dest, src) \
            xstr_assign(dest, xstr_data(src), xstr_size(src))
/* prepend 'xstr' to the begin. */
#define xstr_prepend_str(dest, src) \
            xstr_prepend(dest, xstr_data(src), xstr_size(src))
/* append a 'xstr' to the end. */
#define xstr_append_str(dest, src) \
            xstr_append(dest, xstr_data(src), xstr_size(src))
/* append a 'xstr' starting at 'pos'. */
#define xstr_append_str_at(dest, pos, src) \
            xstr_append_at(dest, pos, xstr_data(src), xstr_size(src))
/* insert a 'xstr' at 'pos'. */
#define xstr_insert_str(dest, pos, src) \
            xstr_insert(dest, pos, xstr_data(src), xstr_size(src))

extern const char g_xstr_i2c_table[];
extern const unsigned char g_xstr_c2i_table[];

/* unsigned int -> string, return a pointer pointed to 'buf'.
 * 'buf' size 36 may be the best.
 * radix: 2 ~ 36.
 * ex:
 *     val = 65535, radix = 10 -> buf = "65535".
 *     val = 65535, radix = 16 -> buf = "FFFF". */
char* uitoa(char* buf, unsigned int val, int radix);
/* unsigned char -> hex string, return a pointer pointed to 'buf'. */
static inline void uctoa_hex(char buf[2], unsigned char val)
{
    buf[0] = g_xstr_i2c_table[val >> 4];
    buf[1] = g_xstr_i2c_table[val & 15];
}

/* string -> unsigned int.
 * base: 2 ~ 36. */
unsigned int atoui(const char* str, int base);
/* hex string -> unsigned char. */
static inline unsigned char atouc_hex(const char str[2])
{
    return g_xstr_c2i_table[(unsigned char)str[1]]
            | (g_xstr_c2i_table[(unsigned char)str[0]] << 4);
}

#endif // _XSTRING_H_