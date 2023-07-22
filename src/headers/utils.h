#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
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

#define NUM_ITER 100
#define SEED 123456789
#define MAX_LEN 1024


#define MALLOC_HANDLER(p) \
    if(p == NULL){ \
        fprintf(stderr, "Malloc failed.\n"); \
        exit(-1); \
    }

#define GET_LAST_NODE(head, prev) \
    prev = head;                              \
    while(prev->next != NULL) prev = prev->next;

#define REMOVE_MID_NODE(idx, curr, prev, next) \
    do{                                              \
        int i = 0;              \
        while (i < idx+1 && next != NULL) {                           \
            prev = curr;                               \
            curr = next;                      \
            next = curr->next;                         \
            i++;                                          \
        }                                             \
        prev->next = next;                     \
    }while(0);

#define GET_SMALLEST(values, len, res)  \
    res = values[0]; \
    for(int i = 1; i < len; ++i)  { \
        res = (values[i] < res) ? values[i] : res; \
    }

#define print_transplants_res(header, list, ch) \
    fprintf(ch, "%s\t\tSuccessful: %f\n\t\tRejected: %f\n", header, list[success], list[reject]);

#define print_by_blood_type(header, list, ch) \
    fprintf(ch, "%s\t\tO: %f\n\t\tA: %f\n\t\tB: %f\n\t\tAB: %f\n", header, list[O], list[A], list[B], list[AB]);

#define print_by_all(header, queues, ch) \
    fprintf(ch, "%s", header); \
    for (int b = 0; b < NUM_BLOOD_TYPES; ++b) { \
        fprintf(ch, "\t\t%s: \n\t\t\tCritical: %f\n\t\t\tNormal: %f\n\t\t\tLow: %f\n",   \
               bt_to_str[b],    \
               queues[b][critical], \
               queues[b][normal],   \
               queues[b][low]); \
    }

#define OPEN_FILE(f, path) \
    if((f = fopen(path, "w")) == NULL) {                                     \
        fprintf(stderr, "Cannot open output file");                          \
        exit(EXIT_FAILURE);                                                  \
    }
//-----------------------------------------------------------------------------

void saveResultsCsv(stats *statistics);
void printResults(stats *statistics, FILE* ch);
void cleanUpEventList(event_list *events);
void cleanUpStatistics(stats *statistics);
void cleanUpTimeStatistics(time_integrated_stats *ti_stats);

#endif //PMCSN_PROJECT_UTILS_H