#include <stdio.h>
#include <string.h>

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
    for (int i = 0; i < xh->bkt_size; ++i)
    {
        int cnt = 0;
        xhash_node_t* p = xh->buckets[i];
        while (p)
        {
            p = p->next;
            ++cnt;
        }
        printf("%d ", cnt);
    }
    printf("\n");
}

void test()
{
    xhash_t* xh = xhash_new(-1, sizeof(mystruct_t),
                        mystruct_hash, mystruct_equal, NULL);
    mystruct_t myst;
    xhash_iter_t iter;

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

    xhash_dump(xh);

    // find all elements
    for (int i = BEGIN_VALUE; i < END_VALUE; ++i)
    {
        sprintf(myst.key, "%d", i);

        iter = xhash_get(xh, &myst);
        if (xhash_iter_valid(iter)) ;/*
            printf("found key = \"%s\", value = %d\n", 
                    ((mystruct_t*)xhash_iter_data(iter))->key,
                    ((mystruct_t*)xhash_iter_data(iter))->value);*/
        else
            printf("key \"%s\" not found!\n", myst.key);
    }
#undef BEGIN_VALUE
#undef END_VALUE
    // find element in another way!
    // but this only work when 'key' is the first member in 'mystruct_t'!
    iter = xhash_get(xh, "311111900");
    if (xhash_iter_valid(iter))
        printf("found!!!!!!!!!! %d\n", ((mystruct_t*)xhash_iter_data(iter))->value);

    // traverse
    for (xhash_iter_t iter = xhash_begin(xh);
            xhash_iter_valid(iter); iter = xhash_iter_next(xh, iter))
    {
        printf("%d ", ((mystruct_t*)xhash_iter_data(iter))->value);
    }
    printf("\n");

    xhash_free(xh);
}

int main(int argc, char** argv)
{
    test();
    return 0;
}