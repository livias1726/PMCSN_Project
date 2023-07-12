#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H
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
#include "init.h"
#include "time.h"
#include "rgf.h"
#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
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


#endif //PMCSN_PROJECT_UTILS_H