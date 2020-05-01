#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xstring.h"

void test()
{
    xstr_t* xs = xstr_new(-1);

    xstr_append(xs, "01234abcdefg", -1);
    xstr_append(xs, "garghwerh4w3y3twyhgwe56u34;'n", -1);
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs),
            xstr_data(xs));

    xstr_assign_at(xs, 1, "aaaaaa", -1);
    printf("assign_at_1_x\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_insert(xs, 3, "qwerty", -1);
    printf("insert_3_x\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_erase(xs, 5, 2);
    printf("erase_5_2\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_push_back(xs, 'o');
    printf("push_back\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_assign(xs, "gwheoihfvnwebqo9083u", -1);
    printf("assign_x\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_prepend(xs, "==========================", -1);
    printf("prepend_x\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs), xstr_capacity(xs), xstr_data(xs));

    xstr_t* xs1 = xstr_new_with_str(xs);
    printf("new_with_str\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs1), xstr_capacity(xs1), xstr_data(xs1));
    xstr_erase(xs, 0, 8);
    xstr_pop_back(xs);
    xstr_prepend_str(xs1, xs);
    printf("prepend_str\n");
    printf("[%ld/%ld] %s\n", xstr_size(xs1), xstr_capacity(xs1), xstr_data(xs1));
    xstr_free(xs1);

    xstr_free(xs);
}

void test1()
{
    char buf[64];
    char* p;

    ultoa(buf, 34253256, 10);
    printf("[str] %s\n", buf);

    strcat(buf, "16.,ewiuhfo");
    printf("[num] %lu\n", atoul(buf, &p, 10));
    printf("p -> '%s'\n", p);

    uctoa_hex(buf, 0xAB);
    buf[2] = '\0';
    printf("[hex str] %s\n", buf);

    printf("[hex num] %d\n", atouc_hex(buf));
}

int main(int argc, char** argv)
{
    test();
    test1();
    return 0;
}