#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xset.h"

/* +++++++++++++++++++++test++++++++++++++++++++++  */
int on_cmp(void* l, void* r)
{
    return strcmp(*(char**)l, *(char**)r);
}
void on_destroy(void* pstr)
{
    printf("free [%s]\n", *(char**)pstr);
    free(*(char**)pstr);
}
void traverse(xset_t* xs)
{
    xset_iter_t iter = xset_begin(xs);
    char** pstr;

    printf("traverse size=%ld\n", xset_size(xs));
    while (xset_iter_valid(iter))
    {
        pstr = xset_iter_key(iter);
        printf(" [%s], ", *pstr);
        iter = xset_iter_next(iter);
    }
    printf("\n");
}
void test()
{
    xset_t* xs = xset_new(sizeof(char*), on_cmp, on_destroy);
    char* str;

    str = strdup("fweogew");
    xset_insert(xs, &str);
    str = strdup("he543yh");
    xset_insert(xs, &str);
    str = strdup("hb353uyh4j");
    xset_insert(xs, &str);
    str = strdup("hb23gr26ty54u");
    xset_insert(xs, &str);
    str = strdup("234235fsd");
    xset_insert(xs, &str);
    str = strdup("u656i5kk");
    xset_insert(xs, &str);
    str = strdup("yh35");
    xset_insert(xs, &str);
    str = strdup("2");
    xset_insert(xs, &str);

    traverse(xs);

    str = "u656i5kk";
    xset_iter_t iter = xset_find(xs, &str);
    if (xset_iter_valid(iter))
    {
        char** pstr = xset_iter_key(iter);
        printf("erase [%s]\n", *pstr);
        xset_erase(xs, iter);
        traverse(xs);
    }

    xset_free(xs);
}
/*----------------------test----------------------*/

/* +++++++++++++++++++++testspeed++++++++++++++++++++++  */
#include <sys/time.h>

int on_cmp2(void* l, void* r)
{
    return strcmp(l, r);
}
void traverse2(xset_node_t* node, int depth, int acc[])
{
    ++acc[depth];
    if (node->rb_left)
        traverse2(node->rb_left, depth + 1, acc);
    if (node->rb_right)
        traverse2(node->rb_right, depth + 1, acc);
}
void test_speed()
{
    const char* char_table = "qwertyuiopasdfghjklzxcvbnm";
#define STR_MAXLEN 16
    xset_t* xs = xset_new(STR_MAXLEN, on_cmp2, NULL);
    char str[STR_MAXLEN];
    char str_to_find[100][STR_MAXLEN];

    srand(31430);
    // generate 100000 random strings and insert into 'xs'
    // store 100 strings into 'str_to_find'
    for (int i = 0; i < 100000; ++i)
    {
        for (int j = 0; j < STR_MAXLEN - 1; ++j)
        {
            str[j] = char_table[rand() % 26];
        }
        str[STR_MAXLEN - 1] = '\0';
        xset_insert(xs, str);

        if (i % 1000 == 0)
            memcpy(str_to_find[i / 1000], str, 16);
    }
    printf("size = %ld\n", xset_size(xs));
#undef STR_MAXLEN

    // printf the number of nodes in every depth of rbtree
    int acc[36] = { 0 };
    traverse2(xs->root, 0, acc);
    printf("nodes[depth] = ");
    for (int depth = 0; acc[depth] > 0; ++depth)
        printf("%d ", acc[depth]);
    printf("\n");

    // search time test
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    for (int i = 0; i < 100; ++i)
        if (!xset_find(xs, str)) printf("%s not found\n", str_to_find[i]);
    gettimeofday(&end, NULL);
    printf("search 100 strings done, time %lds %ldus\n",
                end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);

    xset_free(xs);
}
/*----------------------testspeed----------------------*/

int main(int argc, char** argv)
{
    // test();
    test_speed();
    return 0;
}