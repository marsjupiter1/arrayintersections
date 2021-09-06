//gcc bad.cpp -lstdc++
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <iterator>
#include "common.h"
#include <time.h>

using namespace std;

int main(int args, char **argc)
{
    clock_t start, end;
    double cpu_time_used;
    uint32_t *array1 = GetRandomArray(LONG_LENGTH);
    uint32_t *array2 = GetRandomArray(LONG_LENGTH);
    uint32_t *array3 = (uint32_t *)malloc(LONG_LENGTH * sizeof(uint32_t));

    // we want to time the actual work part
    start = clock();
    set<uint32_t> set1;

    for (int i = 0; i < LONG_LENGTH; i++)
        set1.insert(array1[i]);

    int finds = 0;
    for (int i = 0; i < LONG_LENGTH; i++)
    {
        if (set1.find(array2[i]) != set1.end())
            array3[finds++] = array1[i];
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Total %d Intersect %d time %f seconds\n", LONG_LENGTH, finds, cpu_time_used);
}