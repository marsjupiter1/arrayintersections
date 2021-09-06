#include <cstdio>
#include <xmmintrin.h>
#include <stdint.h>
#include <cstring>

// bits set in the range of numbers 0 - 15
static uint32_t mask_count[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

int main(int args, char **argc)
{
     uint32_t x[] = {0, 1, 3, 4};
    uint32_t y[] = {1, 2, 3, 6};

// the functions that use this need it as a constant known at compile time
#define rotate _MM_SHUFFLE(0, 3, 2, 1)

    // load the array into a 128 bit variable
    __m128i a1 = _mm_loadu_si128((__m128i *)x);
    __m128i a2 = _mm_loadu_si128((__m128i *)y);

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

    printf("x{%d,%d,%d,%d} intersect y{%d,%d,%d,%d}\n", x[0], x[1], x[2], x[3], y[0], y[1], y[2], y[3]);
    printf("mask %d%d%d%d\n",(mask&1)>0?1:0,(mask&2)>0?1:0,(mask&4)>0?1:0,(mask&8)>0?1:0);

    if ((mask & 1)>0) printf("Found %d\n",y[0]);
    if ((mask & 2)>0) printf("Found %d\n",y[1]);
    if ((mask & 4)>0) printf("Found %d\n",y[2]);
    if ((mask & 8)>0) printf("Found %d\n",y[3]);

    // how many populated bits?
    uint32_t matched = mask_count[mask];
    
    printf("%d matches\n",matched);
}

 

