#include "headers/utils.h"

bool check_init(void *p) {
    if (p != NULL) {
        return true;
    }

    return false;
}

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
            MALLOC_HANDLER((*pQueue)->priority_queue[i]->queue);
        }
        (*pQueue)->priority_queue[i]->priority = (PRIORITY)i+1; // skip default priority
    }
}

// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initialize_waiting_list() {
    patient_waiting_list waitingList;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waitingList.blood_type_queues[i] = NULL;
    }
    waitingList.total_number = 0.0;
    return waitingList;
}

organ_bank initialize_organ_bank() {
    organ_bank organBank;
    for (int i = 0; i < NUM_ORGAN_QUEUES; ++i) {
        organBank.queues[i] = NULL;
    }
    organBank.total_number = 0.0;
    return organBank;
}

transplant initialize_transplant_center() {
    transplant transplantCenter;
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