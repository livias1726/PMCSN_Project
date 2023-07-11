#include "headers/utils.h"


void initializeOrganQueue(organ_queue **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(organ_queue));
        MALLOC_HANDLER(*pQueue)
        (*pQueue)->bt = bloodType;
    }

    if ((*pQueue)->queue == NULL) {
        (*pQueue)->queue = malloc(sizeof(organ));
        MALLOC_HANDLER((*pQueue)->queue)
    }
}

void initializePatientQueue(patient_queue_blood_type **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(patient_queue_blood_type));
        MALLOC_HANDLER(pQueue)
        (*pQueue)->bt = bloodType;
    }

    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        if ((*pQueue)->priority_queue[i] == NULL) {
            (*pQueue)->priority_queue[i] = malloc(sizeof(patient_queue_priority));
            MALLOC_HANDLER((*pQueue)->priority_queue[i])
        }
        if ((*pQueue)->priority_queue[i]->queue == NULL) {
            (*pQueue)->priority_queue[i]->queue = malloc(sizeof(patient));
            MALLOC_HANDLER((*pQueue)->priority_queue[i]->queue)
        }
        (*pQueue)->priority_queue[i]->priority = (PRIORITY)i+1; // skip default priority
    }
}

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
    if (transplantCenter.matched_list == NULL) {
        transplantCenter.matched_list = malloc(sizeof(matched));
    }
    transplantCenter.total_number = 0.0;
    transplantCenter.serviceTime = 0.0;
    return transplantCenter;
}

activation initialize_activation_center() {
    activation activationCenter;
    activationCenter.total_number = 0.0;
    activationCenter.serviceTime = 0.0;
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