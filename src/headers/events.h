#ifndef PMCSN_PROJECT_EVENTS_H
#define PMCSN_PROJECT_EVENTS_H

/**
 * This file contains the definitions of the structs representing the events of the simulation (arrivals and terminations).
 * It also defines the stream for the RNGs and declares a function prototype, used to generate a new arrival in the system.
 */


//TODO: don't know if an enum is of any use rather than macro definitions for each stream
typedef enum streams {
    MEAN_PATIENTS_INTERARRIVAL_STREAM, //TODO: to be divided by blood type and urgency??
    MEAN_ORGANS_INTERARRIVAL_STREAM, //TODO: to be divided by blood type??
    MATCHING_TIME_STREAM, //TODO: could be the same as the MEAN_ORGANS_INTERARRIVAL_STREAM if patients queues are never empty
    USER_PROBABILITY_STREAM,
    MEAN_WAITING_TIME_CRITICAL_STREAM, //TODO: could be the same as the matching time but for critical patients
    MEAN_WAITING_TIME_NORMAL_STREAM,
    MEAN_WAITING_TIME_LOW_STREAM,
    MEAN_THINK_TIME_STREAM,
} streams;

/* TODO: to be adapted
typedef struct __arrival{
    double time;                // time of the arrival
    Job job;                    // job associated to the event
    center center;              // ID of the center receiving the arrival
    struct __arrival *next;     // Pointer to next node of the queue
}arrival;

typedef struct __termination{
    double time;                // time of the termination of the job
    Job job;                    // job associated to the event
    center center;              // ID of the center that processed the job
    int server_index;           // Index of the server that becomes idle. This value is not set for digest calculation node
    struct __termination *next; // Pointer to next node of the queue
}termination;

typedef struct __event_list
{
    arrival *arrivals;          // list of arrival events; it's an ordered by time list
    termination *terminations;  // list of termination events; it's an ordered by time list
} event_list;

arrival *getArrival(double simulationTime);
*/

/***
 * --------------------------------------------- EVENT HANDLERS PROTOTYPES ---------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */
void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank* bank);
void addOrganToQueue(organ_queue **pQueue, BLOOD_TYPE bloodType);

/***
 * PATIENT ARRIVAL
 */
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list);
void addPatientToBloodQueue(patient_queue_blood_type **pQueue, PRIORITY priority);
void addPatientToPriorityQueue(patient_queue_priority **pQueuePriority, PRIORITY priority);

/***
 * ORGAN RENEGE
 */
void handleOrganRenege(BLOOD_TYPE bloodType, organ_bank *pBank);
void removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue);

/***
 * PATIENT DEATH
 */
void handlePatientDeath(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list);

/***
 * PATIENT RENEGE
 */
void handlePatientRenege(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list);
void removePatientsFromBloodQueue(LOSS_REASON reason, PRIORITY priority, patient_queue_blood_type **pQueue);
void removePatientsFromPriorityQueue(LOSS_REASON reason, patient_queue_priority **pQueuePriority);

#endif