#include <stdio.h>
#include <stdlib.h>
#include "model.h"

// ------------------- MACROS ---------------------------
//TODO: uvs.c should perform all these
#define GET_MEAN(a, b, i) ((a-b)/i)
#define GET_VAR(a, b, aux) (aux * pow((a - b),2))
#define GET_CONF(a, i, cv) ((cv * sqrt(a / i))/sqrt(i-1))

/**
 * Report of the system statistics given an allocation policy
 * */
typedef struct statistics{
    POLICY policy;

    int numTransplants[NUM_PATIENT_QUEUES];         // number of performed transplants
    int numDeaths[NUM_PATIENT_QUEUES];              // number of death occurred in each waiting list
    int numOrganOut[NUM_ORGAN_QUEUES];              // number of organ outdatings

    double waitTime[NUM_PATIENT_QUEUES];                    // avg time in each waiting list
    double avgWaitingPatients[NUM_PATIENT_QUEUES];          // avg number of patients in each waiting list
    double patientsInterarrivalTime[NUM_PATIENT_QUEUES];    // avg patients interarrival time
    double organsInterarrivalTime[NUM_ORGAN_QUEUES];        // avg organs interarrival time
    double avgActivationDelay;                              // avg delay for temporarily inactive patients
    double utilization;                                     // utilization of the center

    double globalWaitingTime;                       // avg time waiting for a transplant
    double globalWaitingTimeByBloodType;            // avg time waiting for a transplant by blood type
    double globalWaitingTimeByPriority;             // avg time waiting for a transplant by medical urgency
} stats;

// ------------------- PROTOTYPES ----------------
void compute_stats();
void print_stats();