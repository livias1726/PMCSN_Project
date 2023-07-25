#ifndef PMCSN_PROJECT_STATS_H
#define PMCSN_PROJECT_STATS_H

#include "events.h"

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
    area* area_waiting_list[NUM_BLOOD_TYPES][NUM_PRIORITIES];   // time integrated statistics for the waiting list center
    area* area_bank[NUM_BLOOD_TYPES];                           // time integrated statistics for the organ bank center
    area* area_activation;                                      // time integrated statistics for the activation_center center
    area* area_transplant;                                      // time integrated statistics for the transplant center
} time_integrated_stats;

typedef struct waiting_list_stats{
    double num_patient_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];      // total number of patients arrived to the system
    double num_patient_deaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];        // number of death occurred in each waiting list
    double num_patient_reneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];       // number of reneges occurred in each waiting list
    double num_patients_in_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];     // number of patients still in waiting list
    double num_patients_served[NUM_BLOOD_TYPES][NUM_PRIORITIES];       // number of patients matched
    double avg_interarrival_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_wait[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_delay[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_service[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_node[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double utilization[NUM_BLOOD_TYPES][NUM_PRIORITIES];
} waiting_list_stats;

typedef struct organ_bank_stats{
    double num_organ_arrivals[NUM_BLOOD_TYPES];            // total number of organs arrived to the system
    double num_organ_outdatings[NUM_BLOOD_TYPES];          // number of organ outdatings
    double num_organs_in_queue[NUM_BLOOD_TYPES];           // number of organs still in the bank
    double avg_interarrival_time[NUM_BLOOD_TYPES];
    //double avg_wait[NUM_BLOOD_TYPES];
    //double avg_delay[NUM_BLOOD_TYPES];
    //double avg_service[NUM_BLOOD_TYPES];
    //double avg_in_node[NUM_BLOOD_TYPES];
    double avg_in_queue[NUM_BLOOD_TYPES];
} organ_bank_stats;

typedef struct transplant_stats{
    double completed_transplants[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double rejected_transplants[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double rejection_perc[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double avg_in_node;
} transplant_stats;

typedef struct activation_stats{
    double avg_in_node;
    double num_activated;
    double avg_delay;
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
void gatherResults(stats *statistics, event_list *events);
void computeTimeAveragedStats(time_integrated_stats *stats);
void computeTimeAveragedStats2(stats *stats, time_integrated_stats *ti_stats, sim_time *t);

#endif //PMCSN_PROJECT_STATS_H