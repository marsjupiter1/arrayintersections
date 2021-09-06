// gcc -O simd_interesect.cpp -lstdc++
#include <cstdio>
#include <xmmintrin.h>
#include <stdint.h>
#include <cstring>
#include "common.h"
#include <time.h>

using namespace std;

//#define VERIFY
#define SPLITS 20
#define LONG_SPLITS 10
#define MAX_SHORT (LONG_LENGTH / 100)
// bits set in the range of numbers 0 - 15
static uint32_t mask_count[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
// number of elements to move forward if we know the array positions
static uint32_t move_forward[16] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};

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

int simd_scan(uint32_t x[], int long_length, uint32_t y[], int short_length, uint32_t array3[])
{
    uint32_t count = 0;
    int posx = 0;
    int posy = 0;
     // trim lengths to be a multiple of 4 that won't overstep the end
    int x_length = (long_length / 4) * 4 -4 ;
    int y_length = (short_length / 4) * 4-4;

// the functions that use this need it as a constant known at compile time
#define rotate _MM_SHUFFLE(0, 3, 2, 1)
    while (posx < x_length && posy < y_length)
    {
        // load the array into a 128 bit variable
        __m128i a1 = _mm_loadu_si128((__m128i *)&x[posx]);
        __m128i a2 = _mm_loadu_si128((__m128i *)&y[posy]);

        // compare 4 32 bit values
        __m128i match1 = _mm_cmpeq_epi32(a1, a2);

        // rotate the 32 bit values within the 128 variable
        a1 = _mm_shuffle_epi32(a1, rotate);
        __m128i match2 = _mm_cmpeq_epi32(a1, a2);

        a1 = _mm_shuffle_epi32(a1, rotate);
        __m128i match3 = _mm_cmpeq_epi32(a1, a2);

        a1 = _mm_shuffle_epi32(a1, rotate);
        __m128i match4 = _mm_cmpeq_epi32(a1, a2);

        __m128i cmp_mask = _mm_or_si128(_mm_or_si128(match1, match2), _mm_or_si128(match3, match4));

        uint32_t mask = _mm_movemask_ps((__m128)cmp_mask);
        if ((mask & 1) > 0)
        {
            array3[count++] = y[posy];
        }
        if ((mask & 2) > 0)
        {
            array3[count++] = y[posy + 1];
        }
        if ((mask & 4) > 0)
        {
            array3[count++] = y[posy + 2];
        }
        if ((mask & 8) > 0)
        {
            array3[count++] = y[posy + 3];
        }
        
        // the array with the lowest end element can move forward 4
        if (y[posy + 3] > x[posx + 3])
        {
            posx += 4;
            // the y array elements were not shuffled
            // so we know the position of the last match
            posy += move_forward[mask];
        }
        else
        {
            posy += 4;
            // we do notknow the position of the last matched element in the
            // x array but it we had a certain number of matches
            // me must be able to move forward by at least the count of
            // the bits in the mask
            posx += mask_count[mask];
        }
    }

    // remainder

    count += linear_scan(&x[posx], long_length - posx, &y[posy], short_length - posy, &array3[count]);
    return count;
}

int main(int args, char **argc)
{
    clock_t start, end;
    double cpu_time_used1, cpu_time_used2;
    double total_cpu_time_used1=0, total_cpu_time_used2=0;

    assert(LONG_LENGTH / LONG_SPLITS > 0);
    assert(MAX_SHORT / SPLITS > 0);
    for (int long_length = LONG_LENGTH / 10; long_length < LONG_LENGTH; long_length += LONG_LENGTH / LONG_SPLITS)
    {
        uint32_t *array1 = GetRandomArray(long_length, LONG_LENGTH);

        for (int short_length = 1; short_length < MAX_SHORT; short_length += MAX_SHORT / SPLITS)
        {

            uint32_t *array2 = GetRandomArray(short_length, LONG_LENGTH);
            uint32_t *array3 = (uint32_t *)malloc(short_length * sizeof(uint32_t));
            uint32_t *array4 = (uint32_t *)malloc(short_length * sizeof(uint32_t));

            // time an interpolation search
            start = clock();

            int finds1 = simd_scan(array1, long_length, array2, short_length, array3);
            int offset = 0;

            end = clock();
            cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC;
            total_cpu_time_used1+=cpu_time_used1;
            // repeat with a normal search
            start = clock();
            int finds2 = linear_scan(array1, long_length, array2, short_length, array4);

            end = clock();
            cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC;
            total_cpu_time_used2+=cpu_time_used2;
#ifdef VERIFY
            bool bad = false;
            for (int i = 0; i < finds1; i++)
            {
                if (array3[i] != array4[i])
                {
                    bad = true;
                    break;
                }
            }
            if (finds1 != finds2 || bad)
            {
                printf("Error %d != %d %d %d\n", finds1, finds2, short_length, long_length);
                assert(false);
            }
#endif            
            printf("short %d long %d Intersect %d %d linear time %f secs simd time %f secs\n", short_length, long_length, finds2, finds1, cpu_time_used2, cpu_time_used1);

            free(array2);
            free(array3);
            free(array4);
        }
        free(array1);
        //printf("\n");
    }
    printf("Total linear time %f seconds, total simd time %f ratio %f\n",total_cpu_time_used2,total_cpu_time_used1,total_cpu_time_used1/total_cpu_time_used2);
}