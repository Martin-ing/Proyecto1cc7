#include "rand.h"

static unsigned int seed = 12345;

void srand_custom(unsigned int new_seed) {
    seed = new_seed;
}

unsigned int rand_custom(void) {
    seed = seed * 1103515245 + 12345;
    return (seed / 65536) % 32768;
}