#ifndef PMCSN_PROJECT_SIM_H
#define PMCSN_PROJECT_SIM_H

#include "events.h"
#include "stats.h"

#define SEED 7000
#define BATCH_SIZE 365                       // number of days

#define BASE_OBSERVATION 0                  // must be at least 3 years to unblock inactive patients
#define OBSERVATION 150                       // years of system observation after the initial phase
#define START 0.0                           /* initial sim_time                   */
#define STOP (365 * (BASE_OBSERVATION + OBSERVATION))

#define CLOSE_THE_DOOR(events) \
    (events->patient_arrival.inter_arrival_time[O][critical] < STOP) || \
    (events->patient_arrival.inter_arrival_time[A][critical] < STOP) || \
    (events->patient_arrival.inter_arrival_time[B][critical] < STOP)||  \
    (events->patient_arrival.inter_arrival_time[AB][critical] < STOP) ||\
    (events->patient_arrival.inter_arrival_time[O][normal] < STOP) ||   \
    (events->patient_arrival.inter_arrival_time[A][normal] < STOP) ||   \
    (events->patient_arrival.inter_arrival_time[B][normal] < STOP) ||   \
    (events->patient_arrival.inter_arrival_time[AB][normal] < STOP) ||  \
    (events->patient_arrival.total_number > 0) ||                       \
    (events->transplant_arrival.total_number > 0) ||                    \
    (events->activation_arrival.total_number > 0)

void infiniteSim(event_list *events, sim_time *t, time_integrated_stats *ti_stats, stats **batches, stats *final_stat,
                 int *num_iterations);

#endif //PMCSN_PROJECT_SIM_H
