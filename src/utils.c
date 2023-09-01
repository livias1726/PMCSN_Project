#include "headers/utils.h"

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