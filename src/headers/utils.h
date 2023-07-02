#include "stats.h"
#include <math.h>
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"

#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H

#define NUM_ITER 100
#define SEED 123456789

void malloc_handler(int, void**);
void clean_up(int, void**);

#endif //PMCSN_PROJECT_UTILS_H