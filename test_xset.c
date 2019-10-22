#include <stdio.h>
#include "xset.h"

typedef struct
{
    int key;
    int value;
} pair_t;

int pair_cmp(void* l, void* r)
{
    return ((pair_t*)l)->key - ((pair_t*)r)->key;
}

void pair_traverse(xset_t* xs)
{
    xset_iter_t iter = xset_begin(xs);
    pair_t* pair;

    printf("->size=%d\n", xset_size(xs));
    while (xset_iter_valid(iter))
    {
        pair = xset_iter_value(iter);
        printf(" [%d->%d]\n", pair->key, pair->value);
        iter = xset_iter_next(iter);
    }
}

void test()
{
    xset_t* xs = xset_new(sizeof(pair_t), pair_cmp, NULL);
    pair_t pair;

    pair.key = 5467; pair.value = 2563467; xset_insert(xs, &pair);
    pair.key = 74545; pair.value = 2554; xset_insert(xs, &pair);
    pair.key = 1999; pair.value = 43; xset_insert(xs, &pair);
    pair.key = 1991; pair.value = 23; xset_insert(xs, &pair);
    pair.key = 1994; pair.value = 73; xset_insert(xs, &pair);
    pair.key = 19634699; pair.value = 11; xset_insert(xs, &pair);
    pair.key = 1993; pair.value = 22; xset_insert(xs, &pair);
    pair.key = 1699; pair.value = 72; xset_insert(xs, &pair);
    pair.key = 1992; pair.value = 81; xset_insert(xs, &pair);
    pair.key = 234; pair.value = 12; xset_insert(xs, &pair);
    pair.key = 74545; pair.value = 1111; xset_insert(xs, &pair);
    pair.key = 74545; pair.value = 2222; xset_insert(xs, &pair);
    pair.key = 332; pair.value = 11; xset_insert(xs, &pair);
    pair.key = 5525; pair.value = 11; xset_insert(xs, &pair);
    pair.key = 2342; pair.value = 11; xset_insert(xs, &pair);

    pair_traverse(xs);

    pair.key = 1999;
    xset_iter_t iter = xset_find(xs, &pair);
    if (xset_iter_valid(iter))
    {
        pair_t* ppair = xset_iter_value(iter);
        printf("%d->%d\n", pair.key, ppair->value);

        xset_erase(xs, iter);
        pair_traverse(xs);
    }

    xset_free(xs);
}

int main(int argc, char** argv)
{
    test();
    return 0;
}