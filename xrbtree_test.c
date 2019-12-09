#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xrbtree.h"

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
void traverse(xrbtree_t* xs)
{
    xrbtree_iter_t iter = xrbtree_begin(xs);
    char** pstr;

    printf("traverse size=%ld\n", xrbtree_size(xs));
    while (iter != xrbtree_end(xs))
    {
        pstr = xrbtree_iter_data(iter);
        printf(" [%s], ", *pstr);
        iter = xrbtree_iter_next(iter);
    }
    printf("\n");
}
void test()
{
    xrbtree_t* xs = xrbtree_new(sizeof(char*), on_cmp, on_destroy);
    char* str;

    str = strdup("fweogew");
    xrbtree_insert(xs, &str);
    str = strdup("he543yh");
    xrbtree_insert(xs, &str);
    str = strdup("hb353uyh4j");
    xrbtree_insert(xs, &str);
    str = strdup("hb23gr26ty54u");
    xrbtree_insert(xs, &str);
    str = strdup("234235fsd");
    xrbtree_insert(xs, &str);
    str = strdup("u656i5kk");
    xrbtree_insert(xs, &str);
    str = strdup("yh35");
    xrbtree_insert(xs, &str);
    str = strdup("2");
    xrbtree_insert(xs, &str);

    traverse(xs);

    str = "u656i5kk";
    char** pstr = xrbtree_find_ex(xs, &str);
    if (pstr != XRBTREE_INVALID)
    {
        printf("erase [%s]\n", *pstr);
        xrbtree_erase_ex(xs, pstr);
        traverse(xs);
    }

    xrbtree_free(xs);
}
/*----------------------test----------------------*/

/* +++++++++++++++++++++testspeed++++++++++++++++++++++  */
#include <sys/time.h>

int on_cmp2(void* l, void* r)
{
    return strcmp(l, r);
}
void traverse2(xrbtree_node_t* node, int depth, int acc[])
{
    ++acc[depth];
    if (node->rb_left)
        traverse2(node->rb_left, depth + 1, acc);
    if (node->rb_right)
        traverse2(node->rb_right, depth + 1, acc);
}
void tree_overview(xrbtree_t* xs)
{
    // printf the number of nodes in every depth of rbtree
    int acc[36] = { 0 };

    printf("size = %ld\n", xrbtree_size(xs));
    traverse2(xs->root, 0, acc);
    printf("nodes[depth] = ");
    for (int depth = 0; acc[depth] > 0; ++depth)
        printf("%d ", acc[depth]);
    printf("\n");

}
void test_speed()
{
    const char* char_table = "qwertyuiopasdfghjklzxcvbnm";
#define STR_MAXLEN 16
    xrbtree_t* xs = xrbtree_new(STR_MAXLEN, on_cmp2, NULL);
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
        xrbtree_insert(xs, str);

        if (i % 1000 == 0)
            memcpy(str_to_find[i / 1000], str, 16);
    }
#undef STR_MAXLEN
    tree_overview(xs);

    struct timeval begin, end;
    xrbtree_iter_t iter_to_erase[100];

    // search time test
    gettimeofday(&begin, NULL);
    for (int i = 0; i < 100; ++i)
    {
        iter_to_erase[i] = xrbtree_find(xs, str_to_find[i]);
        if (!iter_to_erase[i])
            printf("%s not found\n", str_to_find[i]);
    }
    gettimeofday(&end, NULL);
    printf("search 100 strings done, time %lds %ldus\n",
                end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);

    // remove time test
    gettimeofday(&begin, NULL);
    for (int i = 0; i < 100; ++i)
        xrbtree_erase(xs, iter_to_erase[i]);
    gettimeofday(&end, NULL);
    printf("erase 100 strings done, time %lds %ldus\n",
                end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);
    tree_overview(xs);

    xrbtree_free(xs);
}
/*----------------------testspeed----------------------*/

int main(int argc, char** argv)
{
    test();
    // test_speed();
    return 0;
}