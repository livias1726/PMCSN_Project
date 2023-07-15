#ifndef PMCSN_PROJECT_STATS_H
#define PMCSN_PROJECT_STATS_H

// ------------------- MACROS ---------------------------
//TODO: uvs.c should perform all these
#define GET_MEAN(a, b, i) ((a-b)/i)
#define GET_VAR(a, b, aux) (aux * pow((a - b),2))
#define GET_CONF(a, i, cv) ((cv * sqrt(a / i))/sqrt(i-1))

/**
 * Report of the system statistics given an allocation policy
 * */
typedef struct statistics{
    int numPatientArrivals[NUM_PATIENT_QUEUES];
    int numDeaths[NUM_PATIENT_QUEUES];              // number of death occurred in each waiting list
    int numReneges[NUM_PATIENT_QUEUES];             // number of reneges occurred in each waiting list
    int numPatients[NUM_PATIENT_QUEUES];
    int numOrganArrivals[NUM_ORGAN_QUEUES];
    int numOrganOutdatings[NUM_ORGAN_QUEUES];              // number of organ outdatings
    int numOrgans[NUM_ORGAN_QUEUES];
    int numTransplants[2];         // number of performed transplants

    /* NOT YET USED
    double waitTime[NUM_PATIENT_QUEUES];                    // avg sim_time in each waiting list
    double avgWaitingPatients[NUM_PATIENT_QUEUES];          // avg number of patients in each waiting list
    double patientsInterarrivalTime[NUM_PATIENT_QUEUES];    // avg patients interarrival sim_time
    double organsInterarrivalTime[NUM_ORGAN_QUEUES];        // avg organs interarrival sim_time
    double avgActivationDelay;                              // avg delay for temporarily inactive patients
    double utilization;                                     // utilization of the center
    double globalWaitingTime;                       // avg sim_time waiting for a transplant
    double globalWaitingTimeByBloodType;            // avg sim_time waiting for a transplant by blood type
    double globalWaitingTimeByPriority;             // avg sim_time waiting for a transplant by medical urgency
     */
} stats;

// ------------------- PROTOTYPES ----------------
void compute_stats();
void print_stats();

#endif //PMCSN_PROJECT_STATS_H