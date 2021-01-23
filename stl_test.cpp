#include <iostream>
#include <cstdio>
#include <ctime>
#include <list>
#include <set>
#include <unordered_set>

#define RAND_SEED   123456

// random an integer
static inline int rand_int()
{
    return rand() << 16 | rand() & 0xffff;
}

void test_list_sort(int n)
{
    std::list<int> list;
    clock_t btime, etime;
    int i;

    printf("[test std::list<int> sort]\n");

    srand(RAND_SEED);
    btime = clock();
    for (i = 0; i < n; ++i)
        list.push_back(rand_int());
    etime = clock();
    printf("generate %d random elements done, time %lfs.\n",
        n, (double)(etime - btime) / CLOCKS_PER_SEC);

    btime = clock();
    list.sort();
    etime = clock();
    printf("std::list::sort() done, time %lfs.\n",
        (double)(etime - btime) / CLOCKS_PER_SEC);
}

void test_unordered_set(int n)
{
    std::unordered_set<int> set;
    clock_t begin, end;
    int count, i;

    printf("[test std::unordered_set<int>]\n");

    srand(RAND_SEED);
    // generate 'n' random integer and insert into 'set'
    begin = clock();
    for (i = 0; i < n; ++i)
        set.insert(rand_int());
    end = clock();
    printf("insert %d random integer done, time %lfs, size %ld.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, set.size());

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < n; ++i)
        if (set.find(rand_int()) != set.end()) ++count;
    end = clock();
    printf("search %d random integer done, time %lfs, %d found.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, count);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // remove time test
    begin = clock();
    for (count = 0, i = 0; i < n; ++i)
        if (!set.erase(rand_int())) ++count;
    end = clock();
    printf("remove %d random integer done, time %lfs, %d not found.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, count);
}

void test_set(int n)
{
    std::set<int> set;
    clock_t begin, end;
    int count, i;

    printf("[test std::set<int>]\n");

    srand(RAND_SEED);
    // generate 'n' random integer and insert into 'set'
    begin = clock();
    for (i = 0; i < n; ++i)
        set.insert(rand_int());
    end = clock();
    printf("insert %d random integer done, time %lfs, size %ld.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, set.size());

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // search time test
    begin = clock();
    for (count = 0, i = 0; i < n; ++i)
        if (set.find(rand_int()) != set.end()) ++count;
    end = clock();
    printf("search %d random integer done, time %lfs, %d found.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, count);

    // reset the same seed to get the same series number
    srand(RAND_SEED);
    // remove time test
    begin = clock();
    for (count = 0, i = 0; i < n; ++i)
        if (!set.erase(rand_int())) ++count;
    end = clock();
    printf("remove %d random integer done, time %lfs, %d not found.\n",
            n, (double)(end - begin) / CLOCKS_PER_SEC, count);
}

int main(int argc, char** argv)
{
    int type = 0;

    printf("intput a number:\n"
            "1 - test std::list<int> sort\n"
            "2 - test std::unordered_set<int>\n"
            "3 - test std::set<int>\n");
    if (scanf("%d", &type))
    {
        switch (type)
        {
        case 1: test_list_sort(5000000); break;
        case 2: test_unordered_set(5000000); break;
        case 3: test_set(5000000); break;
        default:
            break;
        }
    }

    return 0;
}