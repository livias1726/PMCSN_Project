#ifndef PMCSN_PROJECT_EVENTS_H
#define PMCSN_PROJECT_EVENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "model.h"
#include "allocation.h"
#include "rgf.h"

#define NUM_EVENTS 7

typedef enum events{
    patient_arrival,
    patient_loss,
    organ_arrival,
    organ_renege,
    transplant,
    activation_arrival,
    activation_completion
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
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, DONOR_TYPE dt);
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt);

// RENEGES
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bt);
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt);

// MATCHING
bool handleMatchingFromOrgan(event_list *events, sim_time *t, organ *organ);
bool handleMatchingFromPatient(event_list *events, sim_time *t, patient *p);

// ACTIVATION
void handlePatientActivation(event_list *events, sim_time *t);

// TRANSPLANT
void handleTransplantCompletion(event_list *events, sim_time *t);

// event utils -------------------------------------------------
organ * removeOrgan(organ_queue *queue, organ_bank *bank);
patient * removePatient(int idx, patient_queue_priority *pp_queue, patient_queue_blood_type *bt_queue,
                        patient_waiting_list *wl);
void addPatientToQueue(event_list *events, sim_time *t, patient_queue_priority *pp_queue,
                       patient_queue_blood_type *bt_queue, patient *p);
void addOrganToQueue(event_list *events, sim_time *t, organ_queue *o_queue, organ *o);
void addToWaitingList(event_list *events, sim_time* t, patient *p);

#endif