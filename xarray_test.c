#include <stdio.h>
#include <string.h>     // memset(), strerror()
#include <stdlib.h>     // malloc()

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

int main(int argc, char** argv)
{
    test();
    return 0;
}
