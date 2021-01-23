#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "xhash.h"

#define RAND_SEED 123456

typedef struct
{
    char key[16];
    int value;
    // ...
} mystruct_t;

unsigned mystruct_hash(void* pdata)
{
    return xhash_string_hash(((mystruct_t*)pdata)->key);
}
int mystruct_equal(void* l, void* r)
{
    return strcmp(((mystruct_t*)l)->key, ((mystruct_t*)r)->key) == 0;
}

// dump the number of node in every bucket
void xhash_dump(xhash_t* xh)
{
    int acc[100] = { 0 };
    int i;

    for (i = 0; i < xh->bkt_size; ++i)
    {
        int cnt = 0;
        xhash_node_t* p = xh->buckets[i];
        while (p)
        {
            p = p->next;
            ++cnt;
        }
        ++acc[cnt];
    }
    for (i = 0; i < 100; ++i)
    {
        if (acc[i])
            printf("the number of buckets which have %d nodes is %d.\n", i, acc[i]);
    }
}

void test()
{
    xhash_t* xh = xhash_new(-1, sizeof(mystruct_t),
                        mystruct_hash, mystruct_equal, NULL);
    mystruct_t  myst;
    mystruct_t* pmyst;
    xhash_iter_t iter;
    int i;

#define BEGIN_VALUE   311111800
#define END_VALUE     311112000
    // put some elements
    for (i = BEGIN_VALUE; i < END_VALUE; ++i)
    {
        sprintf(myst.key, "%d", i);
        myst.value = i;
        xhash_put(xh, &myst);
    }

    xhash_dump(xh);

    // remove some elements
    for (i = BEGIN_VALUE + 111; i < BEGIN_VALUE + 120; ++i)
    {
        sprintf(myst.key, "%d", i);
        xhash_remove(xh, xhash_get(xh, &myst));
    }

    // xhash_dump(xh);

    // find all elements
    for (i = BEGIN_VALUE; i < END_VALUE; ++i)
    {
        sprintf(myst.key, "%d", i);

        pmyst = xhash_get_data(xh, &myst);
        if (pmyst != XHASH_INVALID_DATA)
            ;//printf("found key = \"%s\", value = %d\n", pmyst->key, pmyst->value);
        else
            printf("key \"%s\" not found!\n", myst.key);
    }
#undef BEGIN_VALUE
#undef END_VALUE
    // find element in another way!
    // but this only work when 'key' is the first member in 'mystruct_t'!
    pmyst = xhash_get_data(xh, "311111900");
    if (pmyst != XHASH_INVALID_DATA)
        printf("found!!!!!!!!!! %d\n", pmyst->value);

    // traverse
    for (iter = xhash_begin(xh);
            iter != xhash_end(xh); iter = xhash_iter_next(xh, iter))
    {
        printf("%d ", ((mystruct_t*)xhash_iter_data(iter))->value);
    }
    printf("\n");

    xhash_free(xh);
}

/* ------------------------------------- */

unsigned int_hash(void* v)
{
    return *(unsigned*)v;
}
int int_equal(void* l, void* r)
{
    return *(int*)l == *(int*)r;
}
void test_speed(int nvalues)
{
    xhash_t* xh = xhash_new(-1, sizeof(int), int_hash, int_equal, NULL);
    clock_t begin, end;
    int value, count, i;

    srand(RAND_SEED);
    // generate 'nvalues' random integer and put into 'xh'
    begin = clock();
    for (i = 0; i < nvalues; ++i)
    {
        value = rand() & 0x7fffffff;
        if (!xhash_put(xh, &value))
        {
            printf("out of memory when insert %d value.\n", i);
            break;
        }
    }
    end = clock();
    printf("insert %d random integer done, buckets %u, values %u, time %lfs.\n",
            nvalues, (unsigned)xh->bkt_size, (unsigned)xhash_size(xh), (double)(end - begin) / CLOCKS_PER_SEC);

    xhash_dump(xh);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        value = rand() & 0x7fffffff;
        if (xhash_get(xh, &value))
            ++count;
    }
    end = clock();
    printf("search %d random integer done, time %lfs, found %d.\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // remove time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        value = rand() & 0x7fffffff;
        xhash_iter_t iter = xhash_get(xh, &value);
        if (iter)
            xhash_remove(xh, iter);
        else
            ++count;
    }
    end = clock();
    printf("remove %d random integer done, time %lfs, %d not found.\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    printf("press any key to continue...\n");
    getchar();
    xhash_free(xh);
}

/* ------------------------------------- */
typedef struct {
    short key;
    char value[4096];
} kv_t;

unsigned kv_hash(void* pdata)
{
    return (unsigned) ((kv_t*)pdata)->key;
}
int kv_equal(void* l, void* r)
{
    return ((kv_t*)l)->key == ((kv_t*)r)->key;
}
void test_ex()
{
    xhash_t xh;
    xhash_iter_t iter;
    kv_t* kv;
    short k;

    xhash_init(&xh, -1, sizeof(kv_t), kv_hash, kv_equal, NULL);

    k = 123;
    kv = xhash_iter_data(xhash_put_ex(&xh, &k, sizeof(short)));
    strcpy(kv->value, "string 123");

    k = 234;
    kv = xhash_iter_data(xhash_put_ex(&xh, &k, sizeof(short)));
    strcpy(kv->value, "string 234");

    k = 345;
    kv = xhash_iter_data(xhash_put_ex(&xh, &k, sizeof(short)));
    strcpy(kv->value, "string 345");

    k = 456;
    kv = xhash_iter_data(xhash_put_ex(&xh, &k, sizeof(short)));
    strcpy(kv->value, "string 456");

    k = 234;
    if (xhash_get(&xh, &k))
        printf("key [234] found\n");
    
    for (iter = xhash_begin(&xh);
            iter != xhash_end(); iter = xhash_iter_next(&xh, iter)) {
        kv = xhash_iter_data(iter);
        printf("key [%d], value [%s]\n", kv->key, kv->value);
    }

    xhash_destroy(&xh);
}

/* ------------------------------------- */

int main(int argc, char** argv)
{
    // test();
    // test_ex();
    test_speed(5000000);
    return 0;
}