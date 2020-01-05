#include <stdio.h>
#include <string.h>

#include "xstring.h"

void test()
{
    xstr_t* xs = xstr_new(8);

    xstr_append(xs, "abcdefg", -1);
    xstr_append(xs, "hijklmn", -1);
    xstr_push_back(xs, 'o');
    printf("[%ld] %s\n", xstr_size(xs), xstr_data(xs));

    xstr_erase(xs, 5, 2);
    printf("[%ld] %s\n", xstr_size(xs), xstr_data(xs));

    xstr_assign(xs, "gwheoihfvnwebqo9083u", -1);
    xstr_pop_back(xs);
    printf("[%ld] %s\n", xstr_size(xs), xstr_data(xs));

    xstr_free(xs);
}

void test1()
{
    char buf[64];

    uitoa(buf, 34253256, 10);
    printf("[str] %s\n", buf);

    strcat(buf, "16.,ewiuhfo");
    printf("[num] %u\n", atoui(buf, 10));

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