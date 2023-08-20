#ifndef PMCSN_PROJECT_UTILS_H
#define PMCSN_PROJECT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>

#include "stats.h"

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

#define OPEN_FILE(f, path) \
    if((f = fopen(path, "w")) == NULL) {                                     \
        fprintf(stderr, "Cannot open output file");                          \
        exit(EXIT_FAILURE);                                                  \
    }

#define VALUE(i,j,col) ((i * col) + j)

#define GET_MIN(list, size, min) \
    min = list[0];               \
    for(int z = 1; z < size; ++z)  { \
        if (list[z] < min) min = list[z]; \
    }

#define APPEND_NEW_NODE(head, tail, last, node) \
    if (last == NULL) {                         \
        head->next = node;                      \
    } else {                                    \
        last->next = node;                      \
    }                                           \
    tail = node;

#define RESET_QUEUE(curr, node) \
    while(node != NULL){        \
        curr = node;            \
        node = curr->next;      \
        free(curr);             \
    }

#define RESET_QUEUE_2(curr, node, name) \
    while(node != NULL){                \
        curr = node;                    \
        node = curr->next;              \
        free(curr->name);               \
        free(curr);                     \
    }

static long long random_seed = 0UL;

//-----------------------------------------------------------------------------

void saveResultsCsv(int iter, stats *statistics, bool batch, int batch_num);
void saveResultsLean(stats * statistics);
void cleanUpEventList(event_list *events);
void cleanUpStatistics(stats *statistics);
void cleanUpTimeStatistics(time_integrated_stats *ti_stats);
void new_srand(long long seed);
int new_rand(int module);

#endif //PMCSN_PROJECT_UTILS_H