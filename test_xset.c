#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xset.h"

/* +++++++++++++++++++++test++++++++++++++++++++++  */
int on_cmp(void* l, void* r)
{
    return strcmp(*(char**)l, *(char**)r);
}
void on_destroy(void* pstr)
{
    printf("free [%s]\n", *(char**)pstr);
    free(*(char**)pstr);
}
void traverse(xset_t* xs)
{
    xset_iter_t iter = xset_begin(xs);
    char** pstr;

    printf("traverse size=%ld\n", xset_size(xs));
    while (xset_iter_valid(iter))
    {
        pstr = xset_iter_key(iter);
        printf(" [%s], ", *pstr);
        iter = xset_iter_next(iter);
    }
    printf("\n");
}
void test()
{
    xset_t* xs = xset_new(sizeof(char*), on_cmp, on_destroy);
    char* str;

    str = strdup("fweogew");
    xset_insert(xs, &str);
    str = strdup("he543yh");
    xset_insert(xs, &str);
    str = strdup("hb353uyh4j");
    xset_insert(xs, &str);
    str = strdup("hb23gr26ty54u");
    xset_insert(xs, &str);
    str = strdup("234235fsd");
    xset_insert(xs, &str);
    str = strdup("u656i5kk");
    xset_insert(xs, &str);
    str = strdup("yh35");
    xset_insert(xs, &str);
    str = strdup("2");
    xset_insert(xs, &str);

    traverse(xs);

    str = "u656i5kk";
    xset_iter_t iter = xset_find(xs, &str);
    if (xset_iter_valid(iter))
    {
        char** pstr = xset_iter_key(iter);
        printf("erase [%s]\n", *pstr);
        xset_erase(xs, iter);
        traverse(xs);
    }

    xset_free(xs);
}
/*----------------------test----------------------*/

/* +++++++++++++++++++++testspeed++++++++++++++++++++++  */
#include <sys/time.h>

int on_cmp2(void* l, void* r)
{
    return strcmp(l, r);
}
void test_speed()
{
    const char* char_table = "qwertyuiopasdfghjklzxcvbnm";
#define STR_MAXLEN 16
    xset_t* xs = xset_new(STR_MAXLEN, on_cmp2, NULL);
    char str[STR_MAXLEN];

    srand(31430);
    // generate 10000 random strings and insert into 'xs'
    for (int i = 0; i < 10000; ++i)
    {
        for (int j = 0; j < STR_MAXLEN - 1; ++j)
        {
            str[j] = char_table[rand() % 26];
        }
        str[STR_MAXLEN - 1] = '\0';
        xset_insert(xs, str);
    }
#undef STR_MAXLEN

    printf("xset size = %ld\n", xset_size(xs));

    // search time test
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    xset_iter_t iter = xset_find(xs, "aauqhprhnzrluwn");
    if (xset_iter_valid(iter))
        printf("found %s\n", (char*)xset_iter_key(iter));
    gettimeofday(&end, NULL);
    printf("time %lds %ldus\n", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);

    xset_free(xs);
}
/*----------------------testspeed----------------------*/

int main(int argc, char** argv)
{
    // test();
    test_speed();
    return 0;
}