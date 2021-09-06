//gcc -O interpolation.cpp -lstdc++
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include <time.h>
#include <cassert>

using namespace std;

#define SPLITS 30
#define LONG_SPLITS 4
#define MAX_SHORT (LONG_LENGTH / 1000)

template <typename T>
int interpolation_search(T arr[], int size, T key)
{
    int low = 0;
    int high = size - 1;
    int mid;

    while ((arr[high] != arr[low]) && (key >= arr[low]) && (key <= arr[high]))
    {
        mid = low + ((key - arr[low]) * (high - low) / (arr[high] - arr[low]));

        if (arr[mid] < key)
            low = mid + 1;
        else if (key < arr[mid])
            high = mid - 1;
        else
            return mid;
    }

    if (key == arr[low])
        return low;
    else
        return -1;
}

template <typename T>
int linear_scan(T array1[], int long_length, T array2[], int short_length, T array3[])
{
    int finds2 = 0;
    //printf("long_length %d short_length %d\n",long_length,short_length);
    for (int s1 = 0, s2 = 0; s1 < long_length && s2 < short_length;)
    {
        //printf("compare array1[%d]=%d with array2[%d]=%d\n",s1,array1[s1],s2,array2[s2]);
        if (array1[s1] == array2[s2])
        {
            array3[finds2++] = array1[s1];
            s1++;
            s2++;
        }
        else if (array1[s1] > array2[s2])
        {
            s2++;
        }
        else
        {
            s1++;
        }
    }
    return finds2;
}

int main(int args, char **argc)
{
    clock_t start, end;
    double cpu_time_used1, cpu_time_used2;
    for (int long_length = LONG_LENGTH/10; long_length < LONG_LENGTH; long_length += LONG_LENGTH / LONG_SPLITS)
    {
        uint32_t *array1 = GetRandomArray(long_length,long_length);

        for (int short_length = 1; short_length < MAX_SHORT; short_length += MAX_SHORT / SPLITS)
        {

            uint32_t *array2 = GetRandomArray(short_length,long_length);
            uint32_t *array3 = (uint32_t *)malloc(short_length * sizeof(uint32_t));

            // time an interpolation search
            start = clock();

            int finds1 = 0;
            int offset = 0;
            for (int s2 = 0; s2 < short_length; s2++)
            {
                int found = interpolation_search(&array1[offset], long_length-offset, array2[s2]);
                if (found != -1)
                {
                    array3[finds1++] = array2[s2];
                    offset=found+1;
                }
            }
            end = clock();
            cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC;

            // repeat with a normal search
            start = clock();
            int finds2 = linear_scan(array1, long_length, array2, short_length, array3);

            end = clock();
            cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC;
            if(finds1 != finds2){
                printf("Error %d != %d %d %d\n",finds1,finds2,short_length,long_length);
                assert(false);
            }

            printf("short %d long %d Intersect %d linear time %f secs interpolation time %f secs\n", short_length, long_length,  finds2, cpu_time_used2, cpu_time_used1);

            free(array2);
            free(array3);
        }
        free(array1);
        printf("\n");
    }
}