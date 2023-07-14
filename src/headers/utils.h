#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H
#define NUM_ITER 100
#define SEED 123456789
#define NUM_EVENTS 5
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "enums.h"
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
    for(int i=1; i<len; ++i)  { \
        res = (values[i] < res) ? values[i] : res; \
    }

#endif //PMCSN_PROJECT_UTILS_H