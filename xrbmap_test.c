#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xrbtree.h"

typedef struct mystruct
{
    // key
    char* name;

    // value
    int  age;
    char tag[32];
} mystruct_t;

int mystruct_key_cmp(void* l, void* r)
{
    return strcmp(((mystruct_t*)l)->name, ((mystruct_t*)r)->name);
}
void mystruct_destroy(void* v)
{
    free(((mystruct_t*)v)->name);
}
void insert_mystruct(xrbtree_t* rb, char* name, int age, char tag[16])
{
    mystruct_t myst;
    size_t sz = xrbtree_size(rb);
    
    myst.name = strdup(name);
    myst.age = age;
    strcpy(myst.tag, tag);

    xrbtree_iter_t iter = xrbtree_insert(rb, &myst);
    if (sz == xrbtree_size(rb))
    {
        // when 'name' has already inserted, cover it.
        memcpy(xrbtree_iter_data(iter), &myst, sizeof(myst));
    }
}
void find_mystruct(xrbtree_t* rb, char* name)
{
    // mystruct_t myst = { name };
    // xrbtree_iter_t iter = xrbtree_find(rb, &myst);
    mystruct_t* pmyst = xrbtree_find_ex(rb, &name);

    if (pmyst != XRBTREE_INVALID)
    {
        printf("found [%s]: age=%d, tag=%s\n",
                pmyst->name, pmyst->age, pmyst->tag);
    }
    else
    {
        printf("[%s] not found.\n", name);
    }
}

int main(int argc, char** argv)
{
    xrbtree_t* rb = xrbtree_new(sizeof(mystruct_t),
                        mystruct_key_cmp, mystruct_destroy);

    insert_mystruct(rb, "abcd", 10, "abcd_tag");
    insert_mystruct(rb, "fweg", 23, "fweg_tag");
    insert_mystruct(rb, "gwegvbwhwerh", 12, "gwegvbwhwerh_tag");
    insert_mystruct(rb, "betnenr", 10, "betnenr_tag");
    insert_mystruct(rb, "vaberbn", 64, "vaberbn_tag");
    insert_mystruct(rb, "gweggwrhnerhwrh", 120, "gweggwrhnerhwrh_tag");
    insert_mystruct(rb, "gbwefgwve", 86, "gbwefgwve_tag");
    insert_mystruct(rb, "gbwbewrw", 35, "gbwbewrw_tag");
    insert_mystruct(rb, "wegrebhwe", 46, "wegrebhwe_tag");
    insert_mystruct(rb, "fweg", 29, "fweg_tagx");

    find_mystruct(rb, "wgegfew");
    find_mystruct(rb, "gweggwrhnerhwrh");
    find_mystruct(rb, "fweg");

    xrbtree_free(rb);
    return 0;
}