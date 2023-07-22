#ifndef PMCSN_PROJECT_EVENTS_H
#define PMCSN_PROJECT_EVENTS_H

#include "model.h"

#define NUM_EVENTS 6

typedef enum events{
    patient_arrival,
    patient_loss,
    organ_arrival,
    organ_renege,
    transplant,
    activation
} EVENT;

typedef struct sim_time {
    double current;
    double next;
    double last[NUM_EVENTS];
} sim_time;

/***
 * EVENT LIST: this struct handles every event that can happen in the system and its effects on the model's entities
 */
typedef struct event_list {
    patient_waiting_list patient_arrival;
    activation_center activation_arrival;
    organ_bank organ_arrival;
    double living_donor_completion[NUM_BLOOD_TYPES];
    transplant_center transplant_arrival;
    organs_expired organs_loss;
    patients_lost patients_loss;
} event_list;

// ------------------------------------ Exported EVENT HANDLERS PROTOTYPES ---------------------------------------------

// ARRIVALS
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType, bool living);
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE, PRIORITY);

// RENEGES
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bloodType);
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr);

// ACTIVATION
void handlePatientActivation(event_list *events, sim_time *t);

// TRANSPLANT
void handleTransplantCompletion(event_list *events, sim_time *t);

#endif