#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xlist.h"

void traverse(xlist_t* xl)
{
    xlist_iter_t iter = xlist_begin(xl);

    while (xlist_iter_valid(xl, iter))
    {
        printf("%d ", *(int*)xlist_iter_value(iter));
        iter = xlist_iter_next(iter);
    }
    printf("\n");
}

void on_int_destroy(void* pvalue)
{
    printf("destroy %d.\n", *(int*)pvalue);
}

void test()
{
    xlist_t* xl = xlist_new(sizeof(int), on_int_destroy);
    int v = 0;
    int* pv;

    ++v; xlist_push_back(xl, &v);
    ++v; xlist_push_back(xl, &v);
    ++v; xlist_push_back(xl, &v);
    ++v; xlist_push_back(xl, &v);
    traverse(xl);
    xlist_clear(xl);

    printf("\n");
    ++v; xlist_push_front(xl, &v);
    ++v; xlist_push_front(xl, &v);
    ++v; xlist_insert(xl, xlist_push_front(xl, &v), &v);
    ++v; xlist_push_front(xl, &v);
    traverse(xl);

    pv = xlist_cut_front(xl);
    traverse(xl);
    printf("%d\n", *pv);
    // xlist_cut_free(xl, pv);
    xlist_paste_back(xl, pv);
    traverse(xl);

    printf("\n");
    xlist_free(xl);
}

void test1()
{
    xlist_t xl;
    int v = 100;

    xlist_init(&xl, sizeof(int), on_int_destroy);

    ++v; xlist_push_back(&xl, &v);
    ++v; xlist_push_back(&xl, &v);
    ++v; xlist_push_front(&xl, &v);
    ++v; xlist_push_front(&xl, &v);
    traverse(&xl);

    xlist_pop_back(&xl);
    xlist_pop_back(&xl);
    traverse(&xl);

    xlist_destroy(&xl);
}


int int_cmp(void* l, void* r)
{
    if (*(int*)l > *(int*)r)
        return 1;
    if (*(int*)l < *(int*)r)
        return -1;
    return  0;
}
void test_sort(int n)
{
    xlist_t xl;
    clock_t start, end;

    xlist_init(&xl, sizeof(int), NULL);

    srand(time(NULL));
    for (int i = 0; i < n; ++i)
    {
        int v = rand() & 0x7fffffff;
        xlist_push_back(&xl, &v);
    }
    printf("generate %d random elements done.\n", n);

    start = clock();
    xlist_msort(&xl, int_cmp);
    end = clock();
    printf("sort time %lfs\n",
        (double)(end - start) / CLOCKS_PER_SEC);

    /* check sort result */
    n = 0;
    for (xlist_iter_t i = xlist_begin(&xl);
        i != xlist_end(&xl); i = xlist_iter_next(i))
    {
        int* e = xlist_iter_value(i);
        if (*e < n)
        {
            printf("sort error!\n");
            break;
        }
        n = *e;
    }
    printf("check sort result done.\n");

    xlist_destroy(&xl);
}

int main(int argc, char** argv)
{
    // test();
    // test1();
    test_sort(5000000);
    return 0;
}