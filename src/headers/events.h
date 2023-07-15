#ifndef PMCSN_PROJECT_EVENTS_H
#define PMCSN_PROJECT_EVENTS_H

#define NUM_EVENTS 5

typedef struct sim_time {
    double current;
    double next;
    double last[NUM_EVENTS];
} sim_time;

/***
 * EVENT LIST: this struct handles every event that can happen in the system and its effects on the model's entities
 */
typedef struct event_list {
    struct patient_waiting_list patientArrival;
    struct activation_center activationArrival;
    struct organ_bank organArrival;
    struct transplant_center transplantArrival;
    struct organs_expired_queue organsLoss;
    struct patient_lost_queue patientsLoss;
} event_list;

// ------------------------------------ Exported EVENT HANDLERS PROTOTYPES ---------------------------------------------

// ARRIVALS
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType);
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE, PRIORITY);

// RENEGES
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bloodType);
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr);

// ACTIVATION
void handlePatientActivation(event_list *events, sim_time *t);

// TRANSPLANT
void handleTransplantCompletion(event_list *events, sim_time *t);

#endif