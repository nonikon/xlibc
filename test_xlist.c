#include <stdio.h>
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
    xlist_cut_free(xl, pv);

    printf("\n");
    xlist_free(xl);
}

int main(int argc, char** argv)
{
    test();
    return 0;
}