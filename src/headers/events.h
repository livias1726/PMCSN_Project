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
    MEAN_WAITING_TIME_CRITICAL_STREAM, //TODO: could be the same as the matching sim_time but for critical patients
    MEAN_WAITING_TIME_NORMAL_STREAM,
    MEAN_WAITING_TIME_LOW_STREAM,
    MEAN_THINK_TIME_STREAM,
} streams;

/* TODO: to be adapted
typedef struct __arrival{
    double sim_time;                // sim_time of the arrival
    Job job;                    // job associated to the event
    center center;              // ID of the center receiving the arrival
    struct __arrival *next;     // Pointer to next node of the queue
}arrival;

typedef struct __termination{
    double sim_time;                // sim_time of the termination of the job
    Job job;                    // job associated to the event
    center center;              // ID of the center that processed the job
    int server_index;           // Index of the server that becomes idle. This value is not set for digest calculation node
    struct __termination *next; // Pointer to next node of the queue
}termination;

typedef struct __event_list
{
    arrival *arrivals;          // list of arrival events; it's an ordered by sim_time list
    termination *terminations;  // list of termination events; it's an ordered by sim_time list
} event_list;

arrival *getArrival(double simulationTime);
*/

/***
 * EVENT LIST
 */

typedef struct event_list {
    struct patient_waiting_list patientArrival;
    struct activation_center activationArrival;
    struct organ_bank organArrival;
    struct transplant_center transplantArrival;
    struct organs_expired_queue organsLoss;
    struct patient_lost_queue patientsLoss;
} event_list;

/***
 * --------------------------------------------- EVENT HANDLERS PROTOTYPES ---------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType);
void addOrganToQueue(event_list *events, sim_time *t, organ_queue **pQueue, organ *o);

/***
 * PATIENT ARRIVAL
 */
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE, PRIORITY);
void addToWaitingList(event_list *events, sim_time* t, patient *p);
void addPatientToQueue(event_list *events, sim_time *t, patient_queue_priority **pQueuePriority,
                       patient_queue_blood_type *queueBloodType, patient *p);

/***
 * ORGAN RENEGE
 */
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bloodType);
int removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue, organ_bank *pBank, organs_expired *expiredQueue);

/***
 * PATIENT RENEGE
 */
void
handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr);
void patientLossInternal(event_list *events, sim_time *t, LOSS_REASON reason, patient_queue_priority **pQueuePriority,
                         patient_queue_blood_type *queueBloodType);

/***
 * MATCHING
 */
void handleMatching(event_list *events);

bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE, patient *patient);
bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE, organ *organ);

/***
 * TRANSPLANT
 */
void handleTransplantFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr, organ *o); //DUMMY
void handleTransplantFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *p); //DUMMY

/***
 * ACTIVATION
 */
void handlePatientActivation(event_list *events, sim_time *t);
void addToActivationCenter(event_list *events, sim_time *t, patient *patient);

/***
 * UTILS
 */
void decrementOrgans(organ_queue *organQueue, organ_bank *bank);
void incrementOrgans(organ_queue *pQueue, organ_bank *pBank);
void incrementPatients(patient_queue_priority *pPriority, patient_queue_blood_type *pType, patient_waiting_list *pList);
void decrementPatients(patient_queue_priority *pQueue, patient_queue_blood_type *patientQueueBT, patient_waiting_list *list);
organ * removeOrgan(int idx, organ_queue **pQueue, organ_bank *bank);
patient * removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                        patient_waiting_list *pList);
void addPatientToLost(event_list *events, sim_time *t, patient *p, patients_lost **pQueue, LOSS_REASON reason);
void addOrganToLost(event_list *events, sim_time *t, organ *o, organs_expired **pQueue);
void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient);

#endif