#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xhash.h"

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

#define BEGIN_VALUE   311111800
#define END_VALUE     311112000
    // put some elements
    for (int i = BEGIN_VALUE; i < END_VALUE; ++i)
    {
        sprintf(myst.key, "%d", i);
        myst.value = i;
        xhash_put(xh, &myst);
    }

    xhash_dump(xh);

    // remove some elements
    for (int i = BEGIN_VALUE + 111; i < BEGIN_VALUE + 120; ++i)
    {
        sprintf(myst.key, "%d", i);
        xhash_remove(xh, xhash_get(xh, &myst));
    }

    // xhash_dump(xh);

    // find all elements
    for (int i = BEGIN_VALUE; i < END_VALUE; ++i)
    {
        sprintf(myst.key, "%d", i);

        pmyst = xhash_get_ex(xh, &myst);
        if (pmyst != XHASH_INVALID)
            ;//printf("found key = \"%s\", value = %d\n", pmyst->key, pmyst->value);
        else
            printf("key \"%s\" not found!\n", myst.key);
    }
#undef BEGIN_VALUE
#undef END_VALUE
    // find element in another way!
    // but this only work when 'key' is the first member in 'mystruct_t'!
    pmyst = xhash_get_ex(xh, "311111900");
    if (pmyst != XHASH_INVALID)
        printf("found!!!!!!!!!! %d\n", pmyst->value);

    // traverse
    for (xhash_iter_t iter = xhash_begin(xh);
            iter != xhash_end(xh); iter = xhash_iter_next(xh, iter))
    {
        printf("%d ", ((mystruct_t*)xhash_iter_data(iter))->value);
    }
    printf("\n");

    xhash_free(xh);
}

// random an integer
static inline void rand_int(int* n)
{
    unsigned char* c = (unsigned char*)n;

    c[0] = rand() & 0xff;
    c[1] = rand() & 0xff;
    c[2] = rand() & 0xff;
    c[3] = rand() & 0xff;
}
unsigned int_hash(void* v)
{
    return *(unsigned*)v;
}
int int_equal(void* l, void* r)
{
    return *(int*)l == *(int*)r;
}
#define RAND_SEED 123456
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
        // value = rand();
        rand_int(&value);
        if (!xhash_put(xh, &value))
        {
            printf("out of memory when insert %d value\n", i);
            break;
        }
    }
    end = clock();
    printf("insert %d random integer done, buckets %ld, values %ld, time %lfs\n",
            nvalues, xh->bkt_size, xhash_size(xh), (double)(end - begin) / CLOCKS_PER_SEC);

    xhash_dump(xh);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        if (xhash_get(xh, &value))
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
        xhash_iter_t iter = xhash_get(xh, &value);
        if (iter)
            xhash_remove(xh, iter);
        else
            ++count;
    }
    end = clock();
    printf("search and remove %d random integer done, time %lfs, %d not found!\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);

    xhash_dump(xh);

    getchar();
    xhash_free(xh);
}

int main(int argc, char** argv)
{
    // test();
    test_speed(5000000);
    return 0;
}