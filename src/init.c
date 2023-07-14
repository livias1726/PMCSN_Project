#include "headers/utils.h"

// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initialize_waiting_list() {
    patient_waiting_list waitingList;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waitingList.blood_type_queues[i] = malloc(sizeof(patient_queue_blood_type));
        MALLOC_HANDLER(waitingList.blood_type_queues[i])

        waitingList.blood_type_queues[i]->bt = i;
        waitingList.blood_type_queues[i]->number = 0.0;

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            waitingList.blood_type_queues[i]->priority_queue[j] = malloc(sizeof(patient_queue_priority));
            MALLOC_HANDLER(waitingList.blood_type_queues[i]->priority_queue[j])

            waitingList.blood_type_queues[i]->priority_queue[j]->number = 0.0;
            waitingList.blood_type_queues[i]->priority_queue[j]->priority = j;

            waitingList.blood_type_queues[i]->priority_queue[j]->queue = malloc(sizeof(patient));
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->bt = i;
            MALLOC_HANDLER(waitingList.blood_type_queues[i]->priority_queue[j]->queue)

            waitingList.blood_type_queues[i]->priority_queue[j]->queue->priority = none;
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->is_active = false;
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->next = NULL;
        }
    }
    waitingList.total_number = 0.0;
    return waitingList;
}

organ_bank initialize_organ_bank() {
    organ_bank organBank;
    for (int i = 0; i < NUM_ORGAN_QUEUES; ++i) {
        organBank.queues[i] = malloc(sizeof(organ_queue));
        MALLOC_HANDLER(organBank.queues[i])
        organBank.queues[i]->bt = i;
        organBank.queues[i]->number = 0.0;

        organBank.queues[i]->queue = malloc(sizeof(organ));
        MALLOC_HANDLER(organBank.queues[i]->queue)
        organBank.queues[i]->queue->bt = i;
        organBank.queues[i]->queue->next = NULL;
    }

    organBank.total_number = 0.0;
    return organBank;
}

transplant initialize_transplant_center() {
    transplant transplantCenter;
    transplantCenter.transplanted_patients = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(transplantCenter.transplanted_patients);
    transplantCenter.total_number = 0.0;
    transplantCenter.completed_transplants = 0.0;
    transplantCenter.rejected_transplants = 0.0;
    return transplantCenter;
}

activation initialize_activation_center() {
    activation activationCenter;
    activationCenter.inactive_patients = malloc(sizeof(in_activation));
    MALLOC_HANDLER(activationCenter.inactive_patients);
    activationCenter.total_number = 0.0;
    return activationCenter;
}

organs_expired initialize_organs_expired_queue() {
    organs_expired organsExpired;
    organsExpired.queue = malloc(sizeof(organ));
    MALLOC_HANDLER(organsExpired.queue);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organsExpired.number[i] = 0.0;
    }
    return organsExpired;
}

patients_lost initialize_patient_lost_queue() {
    patients_lost patientsLost;
    patientsLost.queue = malloc(sizeof(patient));
    MALLOC_HANDLER(patientsLost.queue);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            patientsLost.number_dead[i][j] = 0.0;
            patientsLost.number_renege[i][j] = 0.0;
        }
    }
    return patientsLost;
}

patient * new_patient(BLOOD_TYPE bt, PRIORITY pr) {
    patient *new = malloc(sizeof(patient));
    MALLOC_HANDLER(new)
    new->priority = pr;
    new->bt = bt;
    new->is_active = (int) Random() % 2; // FIXME integrate with probability to be inactive and handle it
    new->next = NULL;
    return new;
}

organ * new_organ(BLOOD_TYPE bt) {
    organ *new = malloc(sizeof(organ));
    MALLOC_HANDLER(new)
    new->starting_age = ((int) rand() % (24-0+1)) + 0;
    new->bt = bt;
    new->next = NULL;
    return new;
}

event_list initialize_event_list() {
    patient_waiting_list waiting_list = initialize_waiting_list();
    activation activation_c = initialize_activation_center();
    organ_bank bank = initialize_organ_bank();
    transplant transplant_c = initialize_transplant_center();
    patients_lost patient_loss = initialize_patient_lost_queue();
    organs_expired organs_loss = initialize_organs_expired_queue();

    event_list eventList;
    eventList.organArrival = bank;
    eventList.patientArrival = waiting_list;
    eventList.activationArrival = activation_c;
    eventList.transplantArrival = transplant_c;
    eventList.organsLoss = organs_loss;
    eventList.patientsLoss = patient_loss;
    return eventList;
}

sim_time initialize_time() {
    sim_time t;
    t.current = 0.0;
    t.next = 0.0;
    for(int i=0; i<5; i++) {
        t.last[i] = 0.0;
    }
    return t;
}