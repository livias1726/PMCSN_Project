#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "model.h"
#include "stats.h"
#include "allocation.h"
#include "events.h"
#include "sim.h"
#include "init.h"
#include "rgf.h"
#include "stats.h"
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"

#define MAX_LEN 1024

#define MALLOC_HANDLER(p) \
    if(p == NULL){        \
        fprintf(stderr, "Malloc failed.\n"); \
        exit(-1);         \
    }

#define POP_NODE(head, first) \
    if (first != NULL) {            \
        head->next = first->next;   \
        first->next = NULL;         \
    }

#define REMOVE_MID_NODE(idx, prev, curr, next) \
    int i = 0;                           \
    while (i++ < idx && next != NULL) {  \
        prev = curr;                     \
        curr = next;                     \
        next = curr->next;               \
    }                                    \
    prev->next = next;                   \
    curr->next = NULL;

#define GET_SMALLEST(values, len, res) \
    res = values[0];                   \
    for(int i = 1; i < len; ++i)  {    \
        res = (values[i] < res) ? values[i] : res; \
    }

#define OPEN_FILE(f, path) \
    if((f = fopen(path, "w")) == NULL) {                                     \
        fprintf(stderr, "Cannot open output file");                          \
        exit(EXIT_FAILURE);                                                  \
    }

#define VALUE(i,j,col) ((i * col) + j)

static long long random_seed = 0UL;

//-----------------------------------------------------------------------------

void saveResultsCsv(stats *statistics);
void cleanUpEventList(event_list *events);
void cleanUpStatistics(stats *statistics);
void cleanUpTimeStatistics(time_integrated_stats *ti_stats);
void shuffle(BLOOD_TYPE* , size_t);
void new_srand(long long seed);
int new_rand(int module);

#endif //PMCSN_PROJECT_UTILS_H