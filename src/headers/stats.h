#ifndef PMCSN_PROJECT_STATS_H
#define PMCSN_PROJECT_STATS_H

#include "events.h"

#define LOC 0.95

#define WELFORD(diff, data, mean, sum, n) \
    diff  = data - mean; \
    sum  += diff * diff * (n - 1.0) / n; \
    mean += diff / n;

#define STDEV(tmp, sum, n) \
    tmp = sum;             \
    sum = sqrt(tmp/n);

#define CONFIDENCE(u, t, w, stdev, n) \
    t = idfStudent(n - 1, u);    \
    w = t * stdev / sqrt(n - 1);  \
    stdev = w;

/**
 * Struct of the time averaged statistics for each center
 * */
typedef struct area {
    double node;            // time integrated number in the node
    double queue;           // time integrated number in the queue
    double service;         // time integrated number in service
} area;

// time integrated statistics for each center
typedef struct time_integrated_stats{
    area* area_waiting_list[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    area* area_bank[NUM_BLOOD_TYPES];
    area* area_activation;
    area* area_transplant;
} time_integrated_stats;

typedef struct waiting_list_stats{
    double num_patient_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];      // total number of patients arrived to the system
    double num_patient_deaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];        // number of death occurred in each waiting list
    double num_patient_reneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];       // number of reneges occurred in each waiting list
    double num_patients_in_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];     // number of patients still in waiting list
    double num_patients_served[NUM_BLOOD_TYPES][NUM_PRIORITIES];       // number of patients matched
    double sum_patient_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_patient_deaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_patient_reneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_patient_served[NUM_BLOOD_TYPES][NUM_PRIORITIES];

    // means
    double avg_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_deaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_reneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_interarrival_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_wait[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_delay[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_service[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_node[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double utilization[NUM_BLOOD_TYPES][NUM_PRIORITIES];

    // stddev: used first as a sum container, then for the std deviation and lastly as confidence interval
    double std_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_deaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_reneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_interarrival_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_wait[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_delay[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_service[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_in_node[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_in_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_utilization[NUM_BLOOD_TYPES][NUM_PRIORITIES];
} waiting_list_stats;

typedef struct organ_bank_stats{
    double num_organ_arrivals[NUM_BLOOD_TYPES][2];         // total number of organs arrived to the system
    double num_organs_completions[NUM_BLOOD_TYPES][2];
    double num_organ_outdatings[NUM_BLOOD_TYPES];          // number of organ outdatings
    double num_organs_in_queue[NUM_BLOOD_TYPES];           // number of organs still in the bank
    double sum_organ_arrivals[NUM_BLOOD_TYPES][2];
    double sum_organs_completions[NUM_BLOOD_TYPES][2];
    double sum_organ_outdatings[NUM_BLOOD_TYPES];

    // means
    double avg_arrivals[NUM_BLOOD_TYPES][2];
    double avg_completions[NUM_BLOOD_TYPES][2];
    double avg_outdatings[NUM_BLOOD_TYPES];
    double avg_interarrival_time[NUM_BLOOD_TYPES];
    double avg_wait[NUM_BLOOD_TYPES][2];
    double avg_delay[NUM_BLOOD_TYPES][2];
    double avg_service[NUM_BLOOD_TYPES][2];
    double avg_in_queue[NUM_BLOOD_TYPES];

    // stddev
    double std_arrivals[NUM_BLOOD_TYPES][2];
    double std_completions[NUM_BLOOD_TYPES][2];
    double std_outdatings[NUM_BLOOD_TYPES];
    double std_interarrival_time[NUM_BLOOD_TYPES];
    double std_wait[NUM_BLOOD_TYPES][2];
    double std_delay[NUM_BLOOD_TYPES][2];
    double std_service[NUM_BLOOD_TYPES][2];
    double std_in_queue[NUM_BLOOD_TYPES];
} organ_bank_stats;

typedef struct transplant_stats{
    double num_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double num_transplanted[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double num_rejected[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_transplanted[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double sum_rejected[NUM_BLOOD_TYPES][NUM_PRIORITIES];

    double rejection_perc[NUM_BLOOD_TYPES][NUM_PRIORITIES];

    // mean
    double avg_wait[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_delay[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_service[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_transplanted[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_rejected[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_node;

    // stddev
    double std_wait[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_delay[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_service[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_transplanted[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_rejected[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double std_in_node;
} transplant_stats;

typedef struct activation_stats{
    double num_arrivals[NUM_BLOOD_TYPES];
    double num_deaths[NUM_BLOOD_TYPES];
    double num_reneges[NUM_BLOOD_TYPES];
    double num_activated[NUM_BLOOD_TYPES];
    double sum_arrivals[NUM_BLOOD_TYPES];
    double sum_deaths[NUM_BLOOD_TYPES];
    double sum_reneges[NUM_BLOOD_TYPES];
    double sum_activated[NUM_BLOOD_TYPES];

    // means
    double avg_arrivals[NUM_BLOOD_TYPES];
    double avg_deaths[NUM_BLOOD_TYPES];
    double avg_reneges[NUM_BLOOD_TYPES];
    double avg_activated[NUM_BLOOD_TYPES];
    double avg_wait[NUM_BLOOD_TYPES];
    double avg_delay[NUM_BLOOD_TYPES];
    double avg_service[NUM_BLOOD_TYPES];
    double avg_in_node[NUM_BLOOD_TYPES];

    // stddev: used first as a sum container, then for the std deviation and lastly as confidence interval
    double std_arrivals[NUM_BLOOD_TYPES];
    double std_deaths[NUM_BLOOD_TYPES];
    double std_reneges[NUM_BLOOD_TYPES];
    double std_activated[NUM_BLOOD_TYPES];
    double std_wait[NUM_BLOOD_TYPES];
    double std_delay[NUM_BLOOD_TYPES];
    double std_service[NUM_BLOOD_TYPES];
    double std_in_node[NUM_BLOOD_TYPES];
} activation_stats;

/**
 * Report of the system statistics given an allocation policy
 * */
typedef struct statistics{
    waiting_list_stats* wl_stats;
    organ_bank_stats* ob_stats;
    transplant_stats* trans_stats;
    activation_stats* act_stats;
} stats;

// ------------------- PROTOTYPES ----------------
void gatherResults(stats *statistics, stats *prev_statistics, event_list *events, int iter_num);
void computeTimeAveragedStats(stats *stats, time_integrated_stats *ti_stats, sim_time *t);
void computeTimeAveragedStats2(stats *stats, time_integrated_stats *ti_stats, sim_time *t);
void computeFinalStatistics(stats *final_stat, stats **batches, int num_stats);
void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats);
void welford(int iter, stats *stat, stats *batch);

#endif //PMCSN_PROJECT_STATS_H