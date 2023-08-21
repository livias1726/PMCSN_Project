#include "headers/utils.h"
#include "headers/stats.h"

void shuffle(BLOOD_TYPE *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + new_rand(RAND_MAX) / (RAND_MAX / (n - i) + 1);
            BLOOD_TYPE t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

/**
 * custom pseudo-random number generator to use instead of rand()
 * using a linear congruential generator (x_n = ax_{n-1} + b mod m)
 * */
void new_srand(long long seed){
    random_seed = seed;
}

int new_rand(int module) {
    random_seed = (int) ((1664525UL * random_seed + 1013904223UL) % module);
    return (int)random_seed;
}