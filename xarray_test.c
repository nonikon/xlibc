#include <stdio.h>
#include <string.h>     // memset(), strerror()
#include <stdlib.h>     // malloc()
#include <time.h>       // clock()

#include "xarray.h"

typedef struct
{
    int a, b;
} mystruct_t;

void traverse(xarray_t* array)
{
    for (xarray_iter_t iter = xarray_begin(array);
            iter != xarray_end(array); iter = xarray_iter_next(iter))
    {
        mystruct_t* p = xarray_iter_value(iter);
        printf("array[%d] = %d_%d\n", xarray_iter_index(iter), p->a, p->b);
    }
    printf("blocks = %ld, values = %ld\n", array->blocks, array->values);
}
void on_destroy(void* p)
{
    printf("destroy %d_%d\n", ((mystruct_t*)p)->a, ((mystruct_t*)p)->b);
}

void test()
{
    xarray_t* array = xarray_new(sizeof(mystruct_t), on_destroy);
    mystruct_t myst;

    myst.a = 100; myst.b = 24;
    xarray_set(array, 10024, &myst);
    myst.a = 100; myst.b = 25;
    xarray_set(array, 10025, &myst);
    myst.a = 100; myst.b = 26;
    xarray_set(array, 10026, &myst);
    myst.a = 100; myst.b = 27;
    xarray_set(array, 10027, &myst);
    myst.a = 100; myst.b = 28;
    xarray_set(array, 10028, &myst);
    myst.a = 100; myst.b = 29;
    xarray_set(array, 10029, &myst);
    myst.a = 101; myst.b = 24;
    xarray_set(array, 10024, &myst); // 10024 already inserted
    myst.a = 0; myst.b = 14;
    xarray_set(array, 14, &myst);
    traverse(array);

    printf("\n");
    xarray_unset(array, 10028);
    xarray_unset(array, 10030);
    xarray_unset(array, 14);
    traverse(array);

    mystruct_t* pmyst = xarray_get_ex(array, 10027);
    if (pmyst != XARRAY_INVALID)
        printf("\nfound array[10027] = %d_%d\n", pmyst->a, pmyst->b);

    printf("\n");
    xarray_clear(array);
    traverse(array);

    xarray_free(array);
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
#define RAND_SEED 123456
void test1(int nvalues)
{
    xarray_t* arr = xarray_new(sizeof(unsigned), NULL);
    clock_t begin, end;
    int value, count, i;

    srand(RAND_SEED);
    // generate 'nvalues' random integer and put into 'arr'
    begin = clock();
    for (i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        if (!xarray_set(arr, value, NULL))
        {
            printf("out of memory, i = %d\n", i);
            break;
        }
    }
    end = clock();
    printf("insert %d random integer done, time: %lf\n",
            nvalues, ((double) (end - begin)) / CLOCKS_PER_SEC);
    printf("\tblocks %ld, values %ld\n", arr->blocks, arr->values);
    printf("\tblock memory %ldMB, node memory %ldMB\n",
            sizeof(xarray_block_t) * arr->blocks / 1024 / 1024,
            (sizeof(xarray_node_t) + arr->val_size) * arr->values / 1024 / 1024);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < nvalues; ++i)
    {
        // value = rand();
        rand_int(&value);
        if (xarray_get(arr, value))
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
        xarray_iter_t iter = xarray_get(arr, value);
        if (iter)
            xarray_unset(arr, value);
        else
            ++count;
    }
    end = clock();
    printf("search and remove %d random integer done, time %lfs, %d not found!\n",
            nvalues, (double)(end - begin) / CLOCKS_PER_SEC, count);
    printf("\tblocks %ld, values %ld\n", arr->blocks, arr->values);

    getchar();
    xarray_free(arr);
}

int main(int argc, char** argv)
{
    // test();
    test1(5000000);
    return 0;
}
