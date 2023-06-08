#ifndef PMCSN_PROJECT_MAIN_H
    #define PMCSN_PROJECT_MAIN_H
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"

#define NUM_ITER 100

// ------------------- MACROS ------------------------
#define GET_MEAN(a, b, i) ((a-b)/i)
#define GET_VAR(a, b, aux) (aux * pow((a - b),2))
#define GET_CONF(a, i, cv) ((cv * sqrt(a / i))/sqrt(i-1))

// ---------------- STRUCTS ---------
/**
 * STATS will be used to get every output parameter for a given statistic measure (mean, variance...) related to
 * a certain param (loss, delay, utilization...) obtained for a certain node ()
 * */
typedef struct statistics {
    double param_node;
    //TODO: add fields in order to take every computer param for every modelled node.
} STATS;

// ------------------- PROTOTYPES ----------------
void compute_output_stats(int, STATS*, STATS*, STATS*, STATS*);
void save_output_stats(int, STATS*, STATS*, FILE*);
void print_output_stats(STATS*, STATS*, STATS*);

void malloc_handler(int, void**);
void clean_up(int, void**);