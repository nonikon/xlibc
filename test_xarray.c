#include <stdio.h>
#include <string.h>     // memset(), strerror()
#include <stdlib.h>     // malloc()
#include "xarray.h"

void traverse(xarray_t* array)
{
    xarray_iter_t iter;

    xarray_begin(array, &iter);
    while (xarray_iter_valid(&iter))
    {
        printf("array[%d] = %d\n",
            xarray_iter_index(&iter), *(int*)xarray_iter_value(&iter));
        xarray_iter_next(&iter);
    }
}

void on_int_destroy(void* pvalue)
{
    // printf("%d ", *(int*)pvalue);
}

void test()
{
    xarray_t* array = xarray_new(sizeof(int), on_int_destroy);
    int num = 0;

    ++num; xarray_set(array, 10024, &num);
    ++num; xarray_set(array, 10025, &num);
    ++num; xarray_set(array, 10027, &num);
    ++num; xarray_set(array, 10028, &num);
    ++num; xarray_set(array, 10029, &num);
    ++num; xarray_set(array, 14, &num);
    traverse(array);

    printf("\n");
    xarray_clear(array);
    ++num; xarray_set(array, 9999990, &num);
    ++num; xarray_set(array, 9999991, &num);
    ++num; xarray_set(array, 9999992, &num);
    ++num; xarray_set(array, 9999993, &num);
    ++num; xarray_set(array, 9999994, &num);
    traverse(array);

    printf("\n");
    xarray_unset(array, 9999992);
    xarray_unset(array, 9999995);
    traverse(array);

    printf("\narray[9999990] = %d\n", *(int*)xarray_get(array, 9999990));

    xarray_free(array);
}

int main(int argc, char** argv)
{
    test();
    return 0;
}
