#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "enums.h"
#include "model.h"
#include "stats.h"
#include "allocation.h"
#include "events.h"
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#define NUM_ITER 100
#define SEED 123456789
void malloc_handler(int, void**);
void clean_up(int, void**);
bool check_init(void*);
#endif //PMCSN_PROJECT_UTILS_H