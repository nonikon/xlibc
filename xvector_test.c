#include <stdio.h>
#include <stdlib.h>

#include "xvector.h"

int int_cmp(const void* l, const void* r)
{
    if (*(int*)l > *(int*)r)
        return 1;
    if (*(int*)l < *(int*)r)
        return -1;
    return  0;
}

void on_destroy(void* v)
{
    printf("destroy value %d\n", *(int*)v);
}

void traverse(xvec_t* xv)
{
    int i;
    int* vs = xvec_data(xv);

    printf("[%ld/%ld] ",
            xvec_size(xv), xvec_capacity(xv));
    for (i = 0; i < xvec_size(xv); ++i)
    {
        printf("%d ", vs[i]);
    }
    printf("\n");
}

void test()
{
    xvec_t* xv = xvec_new(4, sizeof(int), on_destroy);
    int vs[4];
    int i, v;

    srand(123456);

    for (i = 0; i < sizeof(vs) / sizeof(vs[0]); ++i)
    {
        vs[i] = rand();
    }
    printf("append %ld values\n", sizeof(vs) / sizeof(vs[0]));
    xvec_append(xv, vs, sizeof(vs) / sizeof(vs[0]));
    traverse(xv);

    v = 6666;
    printf("push_back\n");
    xvec_push_back(xv, &v);
    traverse(xv);
    
    for (i = 0; i < sizeof(vs) / sizeof(vs[0]); ++i)
    {
        vs[i] = rand();
    }
    printf("insert %ld values at pos 1\n", sizeof(vs) / sizeof(vs[0]));
    xvec_insert(xv, 1, vs, sizeof(vs) / sizeof(vs[0]));
    traverse(xv);

    printf("erase 3 values at pos 2\n");
    xvec_erase(xv, 2, 3);
    traverse(xv);
    
    printf("pop_back\n");
    xvec_pop_back(xv);
    traverse(xv);
    
    for (i = 0; i < sizeof(vs) / sizeof(vs[0]); ++i)
    {
        vs[i] = rand();
    }
    printf("prepend %ld values\n", sizeof(vs) / sizeof(vs[0]));
    xvec_prepend(xv, vs, sizeof(vs) / sizeof(vs[0]));
    traverse(xv);

    printf("sort\n");
    qsort(xvec_data(xv), xvec_size(xv), sizeof(int), int_cmp);
    traverse(xv);

    xvec_free(xv);
}

int main(int argc, char** argv)
{
    test();
    return 0;
}