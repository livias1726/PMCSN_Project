#ifndef PMCSN_PROJECT_EVENTS_H
#define PMCSN_PROJECT_EVENTS_H

#define NUM_EVENTS 5

typedef struct sim_time {
    double current;
    double next;
    double last[NUM_EVENTS];
} sim_time;

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
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr);
void patientLossInternal(event_list *events, sim_time *t, LOSS_REASON reason, patient_queue_priority **pQueuePriority,
                         patient_queue_blood_type *queueBloodType);

/***
 * MATCHING
 */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE, patient *patient);
bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE, organ *organ);

/***
 * TRANSPLANT
 */
void handleTransplantFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr, organ *o);
void handleTransplantFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *p);
void handleTransplantCompletion(event_list *events, sim_time *t);
in_transplant *getNextTransplant(double minCompletion, in_transplant *transplanted);
void updateTransplantOffsets(in_transplant *transplanted);

/***
 * ACTIVATION
 */
void handlePatientActivation(event_list *events, sim_time *t);
void addToActivationCenter(event_list *events, sim_time *t, patient *patient);
in_activation *getNextActivation(double minCompletion, in_activation* inactive);
void updateActivationOffsets(in_activation *inactive);

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