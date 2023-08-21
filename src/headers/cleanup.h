#ifndef PMCSN_PROJECT_CLEANUP_H
#define PMCSN_PROJECT_CLEANUP_H

#include "init.h"

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

void cleanUpEventList(event_list *events);
void cleanUpStatistics(stats *statistics);
void cleanUpTimeStatistics(time_integrated_stats *ti_stats);
void resetSimulation(event_list *events, sim_time *sim_time, time_integrated_stats *ti_stats);

#endif //PMCSN_PROJECT_CLEANUP_H
