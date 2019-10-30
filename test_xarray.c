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
        printf("array[%d] = %s\n",
            xarray_iter_index(&iter), (char*)xarray_iter_value(&iter));
        xarray_iter_next(&iter);
    }
    printf("blocks = %d, values = %d\n", array->blocks, array->values);
}

void test()
{
    xarray_t* array = xarray_new(NULL);

    xarray_set(array, 10024, "10024");
    xarray_set(array, 10025, "10025");
    xarray_set(array, 10027, "10027");
    xarray_set(array, 10028, "10028");
    xarray_set(array, 10029, "10029");
    xarray_set(array, 14, "14");
    traverse(array);

    printf("\n");
    xarray_clear(array);
    xarray_set(array, 9999990, "9999990");
    xarray_set(array, 9999991, "9999991");
    xarray_set(array, 9999992, "9999992");
    xarray_set(array, 9999993, "9999993");
    xarray_set(array, 9999994, "9999994");
    traverse(array);

    printf("\n");
    xarray_unset(array, 9999992);
    xarray_unset(array, 9999995);
    traverse(array);

    printf("\narray[9999990] = %s\n", (char*)xarray_get(array, 9999990));

    xarray_free(array);
}

void on_destroy(void* pvalue)
{
    printf("free %s\n", (char*)pvalue);
    free(pvalue);
}
void test2()
{
    xarray_t* array = xarray_new(on_destroy);

    xarray_set(array, 9999990, strdup("9999990"));
    xarray_set(array, 9999991, strdup("9999991"));
    xarray_set(array, 9999992, strdup("9999992"));
    xarray_set(array, 9999993, strdup("9999993"));
    xarray_set(array, 9999990, strdup("9999990x"));
    traverse(array);

    xarray_free(array);
}

int main(int argc, char** argv)
{
    // test();
    test2();
    return 0;
}
