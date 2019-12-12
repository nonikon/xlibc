#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
void traverse(xrbtree_t* rb)
{
    xrbtree_iter_t iter = xrbtree_begin(rb);
    char** pstr;

    printf("traverse size=%ld\n", xrbtree_size(rb));
    while (iter != xrbtree_end(rb))
    {
        pstr = xrbtree_iter_data(iter);
        printf(" [%s], ", *pstr);
        iter = xrbtree_iter_next(iter);
    }
    printf("\n");
}
void test()
{
    xrbtree_t* rb = xrbtree_new(sizeof(char*), on_cmp, on_destroy);
    char* str;

    str = strdup("fweogew");
    xrbtree_insert(rb, &str);
    str = strdup("he543yh");
    xrbtree_insert(rb, &str);
    str = strdup("hb353uyh4j");
    xrbtree_insert(rb, &str);
    str = strdup("hb23gr26ty54u");
    xrbtree_insert(rb, &str);
    str = strdup("234235fsd");
    xrbtree_insert(rb, &str);
    str = strdup("u656i5kk");
    xrbtree_insert(rb, &str);
    str = strdup("yh35");
    xrbtree_insert(rb, &str);
    str = strdup("2");
    xrbtree_insert(rb, &str);

    traverse(rb);

    str = "u656i5kk";
    char** pstr = xrbtree_find_ex(rb, &str);
    if (pstr != XRBTREE_INVALID)
    {
        printf("erase [%s]\n", *pstr);
        xrbtree_erase_ex(rb, pstr);
        traverse(rb);
    }

    xrbtree_free(rb);
}
/*----------------------test----------------------*/

/* +++++++++++++++++++++testspeed++++++++++++++++++++++  */
// random an integer
static inline void rand_int(int* n)
{
    unsigned char* c = (unsigned char*)n;

    c[0] = rand() & 0xff;
    c[1] = rand() & 0xff;
    c[2] = rand() & 0xff;
    c[3] = rand() & 0xff;
}
int on_cmp2(void* l, void* r)
{
    return *(int*)l > *(int*)r ? 1 :
                (*(int*)l < *(int*)r ? -1 : 0);
}
void traverse2(xrbtree_node_t* node, int depth, int acc[])
{
    ++acc[depth];
    if (node->rb_left)
        traverse2(node->rb_left, depth + 1, acc);
    if (node->rb_right)
        traverse2(node->rb_right, depth + 1, acc);
}
void tree_overview(xrbtree_t* rb)
{
    // printf the number of nodes in every depth of rbtree
    int acc[36] = { 0 };
    printf("\tsize %ld, ", xrbtree_size(rb));
    if (rb->root)
        traverse2(rb->root, 0, acc);
    printf("nodes[depth]: ");
    for (int depth = 0; acc[depth] > 0; ++depth)
        printf("%d ", acc[depth]);
    printf(".\n");
}
#define RAND_SEED 123456
void test_speed(int nvalues)
{
    xrbtree_t* rb = xrbtree_new(sizeof(int), on_cmp2, NULL);
    clock_t begin, end;
    int value, count, i;

    srand(RAND_SEED);
    // generate 'nvalues' random integer and insert into 'rb'
    begin = clock();
    for (i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        if (!xrbtree_insert(rb, &value))
        {
            printf("out of memory when insert %d value\n", i);
            break;
        }
    }
    end = clock();
    printf("insert %d random integer done, time %lfs\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC);

    tree_overview(rb);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        if (xrbtree_find(rb, &value))
            ++count;
    }
    end = clock();
    printf("search %d random integer done, time %lfs, found %d\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // remove time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        xrbtree_iter_t iter = xrbtree_find(rb, &value);
        if (iter)
            xrbtree_erase(rb, iter);
        else
            ++count;
    }
    end = clock();
    printf("search and remove %d random integer done, time %lfs, %d not found!\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    tree_overview(rb);

    getchar();
    xrbtree_free(rb);
}
/*----------------------testspeed----------------------*/

int main(int argc, char** argv)
{
    // test();
    test_speed(5000000);
    return 0;
}