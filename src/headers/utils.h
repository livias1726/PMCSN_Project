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
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"

#define NUM_ITER 100
#define SEED 123456789


#define MALLOC_HANDLER(p) \
    if(p == NULL){ \
        fprintf(stderr, "Malloc failed.\n"); \
        exit(-1); \
    }

#define CLEANUP(size, list) \
    do{                     \
        for(int i=0; i<size; ++i){\
            free(list[i]);\
        }               \
    }while(0);

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
    fprintf(ch, "%s\n\t\tSuccessful: %d\n\t\tRejected: %d\n", header, list[0], list[1]);

#define print_by_blood_type(header, list, ch) \
    fprintf(ch, "%s\t\tO: %d\n\t\tA: %d\n\t\tB: %d\n\t\tAB: %d\n", header, list[O], list[A], list[B], list[AB]);

#define print_by_all(header, queues, ch) \
    fprintf(ch, "%s", header); \
    for (int b = 0; b < NUM_BLOOD_TYPES; ++b) { \
        fprintf(ch, "\t\t%s: \n\t\t\tCritical: %d\n\t\t\tNormal: %d\n\t\t\tLow: %d\n",   \
               bt_to_str[b],    \
               queues[b*NUM_PRIORITIES + critical], \
               queues[b*NUM_PRIORITIES + normal],   \
               queues[b*NUM_PRIORITIES + low]); \
    }

//-----------------------------------------------------------------------------

void save_results(stats*);
void print_results(stats*);
void gather_results(stats*, patient_waiting_list, organ_bank, transplant);

#endif //PMCSN_PROJECT_UTILS_H