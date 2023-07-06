#include "headers/utils.h"

/***
 * ------------------------------------------------ EVENT HANDLERS ----------------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                 organ_bank *bank);

void initializePatientQueue(patient_queue_blood_type **pQueue, BLOOD_TYPE bloodType);

void initializeOrganQueue(organ_queue **pQueue, BLOOD_TYPE bloodType);


/***
 * Function to handle the arrival of a new organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank *bank) {
    switch (bloodType) {
        case O:
            addOrganToQueue(&bank->queues[0], O, bank);
            printf("Arrived organ with blood type 0\n");
            break;
        case A:
            addOrganToQueue(&bank->queues[1], A, bank);
            printf("Arrived organ with blood type A\n");
            break;
        case B:
            addOrganToQueue(&bank->queues[2], B, bank);
            printf("Arrived organ with blood type B\n");
            break;
        case AB:
            addOrganToQueue(&bank->queues[3], AB, bank);
            printf("Arrived organ with blood type AB\n");
            break;
        default:
            break;
    }
}

/***
 * Adds an organ to the organ queue with a specific blood type
 * @param pQueue
 * @param bloodType
 */
void addOrganToQueue(organ_queue **pQueue, BLOOD_TYPE bloodType, organ_bank *bank) {
    if ((*pQueue) == NULL) {
        (*pQueue) = (organ_queue*) malloc(sizeof(organ_queue));
        malloc_handler(1, (void *[]) {pQueue});
    }

    organ_queue *organQueue = (*pQueue);

    /* Set queue attributes and increment organs */
    organQueue->bt = bloodType;
    incrementOrgans((*pQueue), bank);


    /* Check if queue is empty */
    if (organQueue->queue == NULL) {
        printf("pointer to queue is NULL: no organs in queue\n");

        /* set list head */
        organQueue->queue = (organ*) malloc(sizeof(organ));
        malloc_handler(1, (void *[]) {organQueue->queue});
        organQueue->queue[0].starting_age = 0; /* Starting age is between 0 and 24 h */ //FIXME substitute rand

        /* add new organ */
        organ *new = (organ*) malloc(sizeof(organ));
        malloc_handler(1, (void *[]) {new});
        organQueue->queue->next = new;
        organQueue->queue->next->starting_age = ((int) rand() % (24-0+1)) + 0; ;
        return;
    }

    /* Add organ to list (not empty) */
    printf("pointer to queue is NOT NULL: there are organs in queue\n");
    organ *o_queue = (*pQueue)->queue;
    while (o_queue->next != NULL) {
        o_queue = o_queue->next;
    }
    o_queue->next = (organ*) malloc(sizeof(organ));
    malloc_handler(1, (void *[]) {o_queue->next});
    o_queue->next->starting_age = ((int) rand() % (24-0+1)) + 0;
}


/***
 * PATIENT ARRIVAL
 */

/***
 * Function to handle the arrival of a new patient in a blood type queue of the waiting list for a transplant
 * @param bloodType
 * @param priority
 * @param list
 */
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list *waitingList) {
    switch (bloodType) {
        case O:
            addPatientToBloodQueue(&waitingList->blood_type_queues[0], priority, waitingList);
            waitingList->blood_type_queues[0]->bt = bloodType;
            printf("Arrived patient with blood type 0\n");
            break;
        case A:
            addPatientToBloodQueue(&waitingList->blood_type_queues[1], priority, waitingList);
            waitingList->blood_type_queues[1]->bt = bloodType;
            printf("Arrived patient with blood type A\n");
            break;
        case B:
            addPatientToBloodQueue(&waitingList->blood_type_queues[2], priority, waitingList);
            waitingList->blood_type_queues[2]->bt = bloodType;
            printf("Arrived patient with blood type B\n");
            break;
        case AB:
            addPatientToBloodQueue(&waitingList->blood_type_queues[3], priority, waitingList);
            waitingList->blood_type_queues[3]->bt = bloodType;
            printf("Arrived organ with blood type AB\n");
            break;
        default:
            break;
    }
}


/***
 * Adds the patient to a specific queue with a specific blood type
 * @param pQueue
 * @param priority
 */
void addPatientToBloodQueue(patient_queue_blood_type **pQueue, PRIORITY priority, patient_waiting_list *waitingList) {
    if ((*pQueue) == NULL) {
        (*pQueue) = (patient_queue_blood_type*) malloc(sizeof(patient_queue_blood_type));
        malloc_handler(1, (void *[]) {*pQueue});
    }
    patient_queue_blood_type *blood_queue = (*pQueue);
    switch (priority) {
        case critical:
            addPatientToPriorityQueue(&blood_queue->priority_queue[0], priority, waitingList,
                                      blood_queue);
            blood_queue->priority_queue[0]->priority = critical;
            printf("Arrived patient with priority Critical\n");
            break;
        case normal:
            addPatientToPriorityQueue(&blood_queue->priority_queue[1], priority, waitingList,
                                      blood_queue);
            blood_queue->priority_queue[1]->priority = normal;
            printf("Arrived patient with priority Normal\n");
            break;
        case low:
            addPatientToPriorityQueue(&blood_queue->priority_queue[2], priority, waitingList,
                                      blood_queue);
            blood_queue->priority_queue[2]->priority = low;
            printf("Arrived patient with priority Low\n");
            break;
        default:
            break;
    }
}

/***
 * Adds the patient to a specific queue with a specific priority
 * @param pQueuePriority
 * @param priority
 */
void addPatientToPriorityQueue(patient_queue_priority **pQueuePriority, PRIORITY priority, patient_waiting_list *waitinglist,
                               patient_queue_blood_type *queueBloodType) {
    if ((*pQueuePriority) == NULL) {
        (*pQueuePriority) = (patient_queue_priority*) malloc(sizeof(patient_queue_priority));
        malloc_handler(1, (void *[]) {*pQueuePriority});
    }

    patient_queue_priority *patient_queue = (*pQueuePriority);
    /* increment patients numbers */
    incrementPatients((*pQueuePriority), queueBloodType, waitinglist);

    if (patient_queue->queue == NULL) {
        printf("pointer to queue is NULL: no patients in queue\n");
        /* set list head */
        patient_queue->queue = (patient*) malloc(sizeof(patient));
        malloc_handler(1, (void *[]) {patient_queue->queue});
        patient_queue->queue[0].priority = none;

        /* add new patient */
        patient *new = (patient*) malloc(sizeof(patient));
        malloc_handler(1, (void *[]) {new});
        patient_queue->queue->next = new;
        patient_queue->queue->next->priority = priority;
        patient_queue->queue->next->is_active = (int) Random() % 2; //FIXME integrate with probability to be inactive and handle it
        return;
    }

    printf("pointer to queue is NOT NULL: there are patients in queue\n");
    patient *p_queue = patient_queue->queue;

    /* Add patient to list (not empty) */
    patient *new = (patient*) malloc(sizeof(patient));
    malloc_handler(1, (void *[]) {new});
    patient_queue->queue->next = new;
    new->priority = priority;
    new->is_active = (int) Random() % 2; //FIXME integrate with probability to be inactive and handle it

    patient *current = p_queue;
    while (current->priority <= new->priority && current->next!=NULL) {
        // Next in the list
        current = current->next;
    }
    current->next = new;
}


/***
 * ORGAN RENEGE
 */

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganRenege(BLOOD_TYPE bloodType, organ_bank *bank) {
    int num_deleted;
    switch (bloodType) {
        case O:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[0]);
            break;
        case A:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[1]);
            break;
        case B:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[2]);
            break;
        case AB:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[3]);
            break;
        default:
            num_deleted = 0;
            break;
    }

    if (num_deleted != 0) {
        bank->total_number = bank->total_number - num_deleted;
    }
}

/***
 * Removes every expired organ from a specific blood type queue in the organ bank
 * @param bloodType
 * @param pQueue
 */
int removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue) {
    organ *o_queue = (*pQueue)->queue;
    organ *current;
    int index, number_deleted = 0;

    current = o_queue;
    while (current->next != NULL) {
        current = current->next;
        if (current->starting_age >= 24) {
            /* The organ has expired */
            removeOrgan(index, pQueue, NULL);
            printf("An organ has expired in the queue with blood type %d\n", bloodType);
            number_deleted++;
        }
    }
    return number_deleted;
}

/***
 * PATIENT DEATH
 */
void handlePatientDeath(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list) {
    list->total_number = list->total_number - 1;
    LOSS_REASON reason = death;
    switch (bloodType) {
        case O:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[0]);
            break;
        case A:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[1]);
            break;
        case B:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[2]);
            break;
        case AB:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[3]);
            break;
        default:
            break;
    }
}

/***
 * PATIENT RENEGE
 */
void handlePatientRenege(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list) {
    list->total_number = list->total_number - 1;
    LOSS_REASON reason = renege;
    switch (bloodType) {
        case O:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[0]);
            break;
        case A:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[1]);
            break;
        case B:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[2]);
            break;
        case AB:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[3]);
            break;
        default:
            break;
    }
}


void removePatientsFromBloodQueue(LOSS_REASON reason, PRIORITY priority, patient_queue_blood_type **pQueue) {
    patient_queue_blood_type *blood_queue = (*pQueue);
    blood_queue->number = blood_queue->number - 1;

    switch (priority) {
        case critical:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[0]);
            break;
        case normal:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[1]);
            break;
        case low:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[2]);
            break;
        default:
            break;
    }
}

void removePatientsFromPriorityQueue(LOSS_REASON reason, patient_queue_priority **pQueuePriority) {
    patient *priority_queue = (*pQueuePriority)->queue;
    int max_number, index;

    if (reason == death) {
        /* Handle patient death from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I remove the oldest one
        removePatient(0, pQueuePriority, NULL, NULL);
        printf("A patient is dead in queue with priority %d\n", priority_queue->priority);
    } else {
        /* Handle patient renege from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I choose a random job
        max_number = (int) (*pQueuePriority)->number-1;
        index = ((int) rand() % (max_number-0+1)) + 0; // FIXME alternative to rand()
        removePatient(index, pQueuePriority, NULL, NULL);
        printf("A patient left the queue with priority %d\n", priority_queue->priority);
    }

    /* Decrement number of patients in priority queue */
    (*pQueuePriority)->number = (*pQueuePriority)->number - 1;
}

/***
 * MATCHING
 */
void handleMatching(POLICY policy, patient_waiting_list *pWaitingList, organ_bank *bank) {
    int patient_queues;
    patient_queue_blood_type *patient_qs[] = {pWaitingList->blood_type_queues[0], pWaitingList->blood_type_queues[1],
                                        pWaitingList->blood_type_queues[2], pWaitingList->blood_type_queues[3]};
    organ_queue *organ_qs[] = {bank->queues[0], bank->queues[1], bank->queues[2], bank->queues[3]};
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if (!check_init(patient_qs[i])) {
            initializePatientQueue(&patient_qs[i], (BLOOD_TYPE)i);
        }
        if (!check_init(organ_qs[i])) {
            initializeOrganQueue(&organ_qs[i], (BLOOD_TYPE)i);
        }
    }

    /* TODO: note: randomically choose the blood type order for matching */
    if (policy == ABO_Comp) {
        /* handle matching with policy ABO-compatible */
        for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
            switch ((BLOOD_TYPE) i) {
                case O:
                    handleMatchingABOCompatible(patient_qs[0], organ_qs[0], pWaitingList, bank); // organ O - patient O
                    handleMatchingABOCompatible(patient_qs[1], organ_qs[0], pWaitingList, bank); // organ O - patient A
                    handleMatchingABOCompatible(patient_qs[2], organ_qs[0], pWaitingList, bank); // organ O - patient B
                    handleMatchingABOCompatible(patient_qs[3], organ_qs[0], pWaitingList, bank); // organ O - patient AB
                    break;
                case A:
                    handleMatchingABOCompatible(patient_qs[1], organ_qs[1], pWaitingList, bank); // organ: A - patient A
                    handleMatchingABOCompatible(patient_qs[3], organ_qs[1], pWaitingList, bank); // organ: A - patient AB
                    break;
                case B:
                    handleMatchingABOCompatible(patient_qs[2], organ_qs[2], pWaitingList, bank); // organ: B - patient B
                    handleMatchingABOCompatible(patient_qs[3], organ_qs[2], pWaitingList, bank); // organ: B - patient B
                    break;
                case AB:
                    handleMatchingABOCompatible(patient_qs[3], organ_qs[3], pWaitingList, bank); // organ: AB - patient AB
                    break;
            }
        }
    } else {
        /* handle matching with policy ABO-identical */
        handleMatchingABOIdentical(patient_qs[0], organ_qs[0], pWaitingList, bank); // O
        handleMatchingABOIdentical(patient_qs[1], organ_qs[1], pWaitingList, bank); // A
        handleMatchingABOIdentical(patient_qs[2], organ_qs[2], pWaitingList, bank); // B
        handleMatchingABOIdentical(patient_qs[3], organ_qs[3], pWaitingList, bank); // AB
    }
}

void handleMatchingABOIdentical(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                organ_bank *bank) {
    if (ABOIdentical(organ_q->bt, patient_q->bt)) {
        while (organ_q->organ_available && patient_q->number > 0) {
            // fino a che ci sono organi disponibili
            while (patient_q->priority_queue[0]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti critici
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[0], patient_q, pList);
            }

            while (patient_q->priority_queue[1]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti normali
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[1], patient_q, pList);
            }

            while (patient_q->priority_queue[2]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti low
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[2], patient_q, pList);
            }
        }
    }
}

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                 organ_bank *bank) {
    if (ABOCompatible(organ_q->bt, patient_q->bt)) {
        while (organ_q->organ_available && patient_q->number > 0) {
            // fino a che ci sono organi disponibili
            while (patient_q->priority_queue[0]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti critici
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[0], patient_q, pList);
            }

            while (patient_q->priority_queue[1]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti normali
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[1], patient_q, pList);
            }

            while (patient_q->priority_queue[2]->number != 0 && organ_q->organ_available) {
                // fino a che ci sono pazienti low
                removeOrgan(0, &organ_q, bank);
                removePatient(0, &patient_q->priority_queue[2], patient_q, pList);
            }
        }
    }

}


/***
 * UTILS
 */

void incrementOrgans(organ_queue *pQueue, organ_bank *pBank) {
    pQueue->number++;
    if (!pQueue->organ_available) {
        pQueue->organ_available = true;
    }
    pBank->total_number++;
}

void decrementOrgans(organ_queue *organQueue, organ_bank *bank) {
    if (organQueue->number > 0) {
        organQueue->number--;
    }
    if (bank->total_number > 0) {
        bank->total_number--;
    }
    if (organQueue->number == 0) {
        organQueue->organ_available = false;
        printf("No organs left in queue with blood type %d\n", organQueue->bt);
    }
}

void incrementPatients(patient_queue_priority *patientQueuePriority, patient_queue_blood_type *patientQueueBloodType,
                       patient_waiting_list *patientWaitingList) {
    patientQueuePriority->number++;
    patientQueueBloodType->number++;
    patientWaitingList->total_number++;
    if (!patientQueueBloodType->patient_available) {
        patientQueueBloodType->patient_available = true;
    }
}

void decrementPatients(patient_queue_priority *patientQueuePriority, patient_queue_blood_type *patientQueueBT,
                       patient_waiting_list *list) {
    if (patientQueuePriority->number > 0) {
        patientQueuePriority->number--;
    }
    if (patientQueueBT->number > 0) {
        patientQueueBT->number--;
    }
    if (list->total_number > 0) {
        list->total_number--;
    }
}

void removeOrgan(int idx, organ_queue **pQueue, organ_bank *bank) {
    int i = 0;
    organ *organs = (*pQueue)->queue;
    organ *current = organs;
    organ *prev = current;
    organ *next = current->next;

    while (i < idx) {
        prev = current;
        current = current->next;
        next = current->next;
        i++;
    }

    prev->next = next;
    decrementOrgans((*pQueue), bank);
}

void removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                   patient_waiting_list *pList) {
    int i = 0;
    patient *patients = (*pQueue)->queue;
    patient *current = patients;
    patient *prev = current;
    patient *next = current->next;

    while (i < idx) {
        prev = current;
        current = current->next;
        next = current->next;
        i++;
    }

    prev->next = next;
    decrementPatients((*pQueue), pQueueBT, pList);
}

double handle_transplant() { return 0; };