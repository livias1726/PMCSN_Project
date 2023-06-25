#include "stats.h"
#include <math.h>
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"

#define NUM_ITER 100
#define SEED 123456789

void malloc_handler(int, void**);
void clean_up(int, void**);