#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "xrbtree.h"

#define RAND_SEED 123456

/* +++++++++++++++++++++test++++++++++++++++++++++  */

typedef struct
{
    char key[36];
    int value;
    // ...
} mystruct_t;

int on_cmp(void* l, void* r)
{
    return strcmp(((mystruct_t*)l)->key, ((mystruct_t*)r)->key);
}
void traverse(xrbt_t* rb)
{
    xrbt_iter_t iter = xrbt_begin(rb);
    mystruct_t* p;

    printf("traverse size = %u\n", (unsigned)xrbt_size(rb));
    while (iter != xrbt_end(rb))
    {
        p = xrbt_iter_data(iter);
        printf("[%s, %d], ", p->key, p->value);
        iter = xrbt_iter_next(iter);
    }
    printf("\n");
}
void test()
{
    xrbt_t* rb = xrbt_new(sizeof(mystruct_t), on_cmp, NULL);
    mystruct_t myst;
    mystruct_t* p;

    strcpy(myst.key, "fweogew");
    myst.value = 1;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "he543yh");
    myst.value = 2;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "hb353uyh4j");
    myst.value = 3;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "hb23gr26ty54u");
    myst.value = 4;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "234235fsd");
    myst.value = 5;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "u656i5kk");
    myst.value = 6;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "yh35");
    myst.value = 7;
    xrbt_insert(rb, &myst);
    strcpy(myst.key, "2");
    myst.value = 8;
    xrbt_insert(rb, &myst);

    traverse(rb);

    strcpy(myst.key, "u656i5kk");
    myst.value = 999;
    // key already exist, 'xrbt_insert' will do nothing (value will not be modified!)
    xrbt_insert(rb, &myst);

    p = xrbt_find_data(rb, &myst);
    if (p != XRBT_INVALID_DATA)
    {
        printf("found [%s, %d], erase\n", p->key, p->value);
        xrbt_erase_data(rb, p);
        traverse(rb);
    }
    
    // !!!!!
    p = xrbt_find_data(rb, "234235fsd");
    if (p != XRBT_INVALID_DATA)
        printf("found [%s, %d]!!!!\n", p->key, p->value);

    xrbt_free(rb);
}
/*----------------------test----------------------*/

/* +++++++++++++++++++++testspeed++++++++++++++++++++++  */
int on_cmp2(void* l, void* r)
{
    return *(int*)l > *(int*)r ? 1 :
                (*(int*)l < *(int*)r ? -1 : 0);
}
void traverse2(xrbt_node_t* node, int depth, int acc[])
{
    ++acc[depth];
    if (node->rb_left)
        traverse2(node->rb_left, depth + 1, acc);
    if (node->rb_right)
        traverse2(node->rb_right, depth + 1, acc);
}
void tree_overview(xrbt_t* rb)
{
    // printf the number of nodes in every depth of rbtree
    int acc[36] = { 0 };
    int depth;
    printf("\tsize %u, ", (unsigned)xrbt_size(rb));
    if (rb->root)
        traverse2(rb->root, 0, acc);
    printf("nodes[depth]: ");
    for (depth = 0; acc[depth] > 0; ++depth)
        printf("%d ", acc[depth]);
    printf(".\n");
}
// random an integer
static inline int rand_int()
{
    return rand() << 16 | rand() & 0xffff;
}
void test_speed(int nvalues)
{
    xrbt_t* rb = xrbt_new(sizeof(int), on_cmp2, NULL);
    clock_t begin, end;
    int value, count, i;

    srand(RAND_SEED);
    // generate 'nvalues' random integer and insert into 'rb'
    begin = clock();
    for (i = 0; i < nvalues; ++i)
    {
        value = rand_int();
        if (!xrbt_insert(rb, &value))
        {
            printf("out of memory when insert %d value.\n", i);
            break;
        }
    }
    end = clock();
    printf("insert %d random integer done, values %u, time %lfs.\n",
            nvalues, (unsigned)xrbt_size(rb), (double)(end - begin) / CLOCKS_PER_SEC);

    tree_overview(rb);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        value = rand_int();
        if (xrbt_find(rb, &value))
            ++count;
    }
    end = clock();
    printf("search %d random integer done, time %lfs, %d found.\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // remove time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        value = rand_int();
        xrbt_iter_t iter = xrbt_find(rb, &value);
        if (iter)
            xrbt_erase(rb, iter);
        else
            ++count;
    }
    end = clock();
    printf("remove %d random integer done, time %lfs, %d not found.\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    printf("press any key to continue...\n");
    getchar();
    xrbt_free(rb);
}
/*----------------------testspeed----------------------*/

int main(int argc, char** argv)
{
    // test();
    test_speed(5000000);
    return 0;
}