#ifndef PMCSN_PROJECT_STATS_H
#define PMCSN_PROJECT_STATS_H

#include "events.h"

// ------------------- MACROS ---------------------------
//TODO: uvs.c should perform all these
#define GET_MEAN(a, b, i) ((a-b)/i)
#define GET_VAR(a, b, aux) (aux * pow((a - b),2))
#define GET_CONF(a, i, cv) ((cv * sqrt(a / i))/sqrt(i-1))
/**
 * Struct of the time averaged statistics for each center
 * */
typedef struct area {
    double node;            // time integrated number in the node
    double queue;           // time integrated number in the queue
    double service;         // time integrated number in service
} area;

/**
 * Report of the system statistics given an allocation policy
 * */
typedef struct statistics{
    // population
    double numOrganArrivals[NUM_BLOOD_TYPES];                       // total number of organs arrived to the system
    double numPatientArrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];     // total number of patients arrived to the system
    double numDeaths[NUM_BLOOD_TYPES][NUM_PRIORITIES];              // number of death occurred in each waiting list
    double numReneges[NUM_BLOOD_TYPES][NUM_PRIORITIES];             // number of reneges occurred in each waiting list
    double numPatients[NUM_BLOOD_TYPES][NUM_PRIORITIES];            // number of patients still in waiting list
    double numOrganOutdatings[NUM_BLOOD_TYPES];                     // number of organ outdatings
    double numOrgans[NUM_BLOOD_TYPES];                              // number of organs still in the bank
    double numTransplants[2];                                       // number of transplants (successful and rejected)

    // time integrated statistics for each center
    area* area_waiting_list;                          // time integrated statistics for the waiting list center
    area* area_activation;                           // time integrated statistics for the activation center
    area* area_bank;                                 // time integrated statistics for the organ bank center
    area* area_transplant;                           // time integrated statistics for the transplant center

    // time                                          // avg sim_time waiting for a transplant
    double meanGlobalWaitingTimePerQueue[NUM_BLOOD_TYPES][NUM_PRIORITIES];  // avg sim_time waiting for a transplant in each queue
    double patientsInterarrivalTime[NUM_BLOOD_TYPES][NUM_PRIORITIES];   // avg patients interarrival sim_time
    double organsInterarrivalTime[NUM_BLOOD_TYPES];                     // avg organs interarrival sim_time

    /* NOT YET USED
    double avgWaitingPatients[NUM_PATIENT_QUEUES];          // avg number of patients in each waiting list
    double avgActivationDelay;                              // avg delay for temporarily inactive patients
    double utilization;                                     // utilization of the center
     */
} stats;

// ------------------- PROTOTYPES ----------------
void computeStats(stats *statistics);
void gatherResults(stats *statistics, event_list *events);

#endif //PMCSN_PROJECT_STATS_H