#include <cassert>

#define LONG_LENGTH 6000000
#define SPARSITY (3/2)

static uint32_t getRandom(int low, int high)
{
    return rand() % (high + 1 - low) + low;
}

int cmpfunc(const void *a, const void *b)
{
    return (*(uint32_t *)a - *(uint32_t *)b);
}

// ordered array of unique random numbers
uint32_t *GetRandomArray(uint32_t length, uint32_t long_length)
{
    uint32_t *array = (uint32_t *)malloc(length * sizeof(uint32_t));
    uint32_t current = 0;
    bool found;

    // if we don't have this, we can never complete
    assert(long_length * SPARSITY>length);
    for (int i = 0; i < length; i++)
    {
        array[i] = getRandom(1, long_length * SPARSITY);
    }
    if (length > 1) do
    {
        qsort(array, length, sizeof(uint32_t), cmpfunc);
        //printf("*[");
        //for (int i = 0;i < length;i++)printf(" %d ",array[i]);
        //printf("]\n");
        found = false;
        // try again with any duplicates
        for (int i = 0; i < length - 1; i++)
        {
            if (array[i] == array[i + 1])
            {
                array[i] = getRandom(1, long_length * SPARSITY);
                found = true;
            }
        }
    } while (found);
    //printf("[");
    //for (int i = 0;i < length;i++)printf(" %d ",array[i]);
    //printf("]\n");
    return array;
}