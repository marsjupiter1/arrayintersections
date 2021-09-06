//gcc -O sensible.cpp -lstdc++
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
    
    int finds = 0;
    for (int s1 = 0,s2=0; s1 < LONG_LENGTH && s2<LONG_LENGTH; )
    {
        if ( array1[s1] ==  array2[s2]){
            array3[finds++] = array1[s1];
            s1++;
            s2++;
        }else if (array1[s1] > array2[s2]){
            s2++;
        }else{
            s1++;
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Total %d Intersect %d time %f seconds\n", LONG_LENGTH, finds, cpu_time_used);
}