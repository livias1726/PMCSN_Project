#include "headers/utils.h"

/***
 * ------------------------------------------------ EVENT HANDLERS ----------------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q);

void initializePatientQueue(patient_queue_blood_type **pQueue, BLOOD_TYPE bloodType);

void initializeOrganQueue(organ_queue **pQueue, BLOOD_TYPE bloodType);

/***
 * Function to handle the arrival of a new organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank *bank) {
    bank->total_number = bank->total_number + 1;
    switch (bloodType) {
        case O:
            addOrganToQueue(&bank->queues[0], O);
            malloc_handler(1, (void *[]) {bank->queues[0]});
            printf("Arrived organ with blood type 0\n");
            break;
        case A:
            addOrganToQueue(&bank->queues[1], A);
            malloc_handler(1, (void *[]) {bank->queues[1]});
            printf("Arrived organ with blood type A\n");
            break;
        case B:
            addOrganToQueue(&bank->queues[2], B);
            malloc_handler(1, (void *[]) {bank->queues[2]});
            printf("Arrived organ with blood type B\n");
            break;
        case AB:
            addOrganToQueue(&bank->queues[3], AB);
            malloc_handler(1, (void *[]) {bank->queues[3]});
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
void addOrganToQueue(organ_queue **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(organ_queue));
    }

    organ_queue *organQueue = (*pQueue);

    /* Set queue attributes */
    organQueue->bt = bloodType;
    organQueue->number = (*pQueue)->number+1;
    organQueue->organ_available = true;

    /* Check if queue is empty */
    if (organQueue->queue == NULL) {
        printf("pointer to queue is NULL: no organs in queue\n");

        /* set list head */
        organQueue->queue = malloc(sizeof(organ));
        organQueue->queue[0].starting_age = 0; /* Starting age is between 0 and 24 h */ //FIXME substitute rand

        /* add new organ */
        organ *new = malloc(sizeof(organ));
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
    o_queue->next = malloc(sizeof(organ));
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
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list *list) {
    list->total_number = list->total_number + 1;
    switch (bloodType) {
        case O:
            addPatientToBloodQueue(&list->blood_type_queues[0], priority);
            malloc_handler(1, (void *[]) {list->blood_type_queues[0]});
            list->blood_type_queues[0]->bt = bloodType;
            printf("Arrived patient with blood type 0\n");
            break;
        case A:
            addPatientToBloodQueue(&list->blood_type_queues[1], priority);
            malloc_handler(1, (void *[]) {list->blood_type_queues[1]});
            list->blood_type_queues[1]->bt = bloodType;
            printf("Arrived patient with blood type A\n");
            break;
        case B:
            addPatientToBloodQueue(&list->blood_type_queues[2], priority);
            malloc_handler(1, (void *[]) {list->blood_type_queues[2]});
            list->blood_type_queues[2]->bt = bloodType;
            printf("Arrived patient with blood type B\n");
            break;
        case AB:
            addPatientToBloodQueue(&list->blood_type_queues[3], priority);
            malloc_handler(1, (void *[]) {list->blood_type_queues[3]});
            list->blood_type_queues[3]->bt = bloodType;
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
void addPatientToBloodQueue(patient_queue_blood_type **pQueue, PRIORITY priority) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(patient_queue_blood_type));
    }
    patient_queue_blood_type *blood_queue = (*pQueue);
    switch (priority) {
        case critical:
            addPatientToPriorityQueue(&blood_queue->priority_queue[0], priority);
            malloc_handler(1, (void *[]) {blood_queue->priority_queue[0]});
            blood_queue->priority_queue[0]->priority = critical;
            /*increment patient to blood queue */
            blood_queue->number++;
            printf("Arrived patient with priority Critical\n");
            break;
        case normal:
            addPatientToPriorityQueue(&blood_queue->priority_queue[1], priority);
            malloc_handler(1, (void *[]) {blood_queue->priority_queue[1]});
            blood_queue->priority_queue[1]->priority = normal;
            /*increment patient to blood queue */
            blood_queue->number++;
            printf("Arrived patient with priority Normal\n");
            break;
        case low:
            addPatientToPriorityQueue(&blood_queue->priority_queue[2], priority);
            malloc_handler(1, (void *[]) {blood_queue->priority_queue[2]});
            blood_queue->priority_queue[2]->priority = low;
            /*increment patient to blood queue */
            blood_queue->number++;
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
void addPatientToPriorityQueue(patient_queue_priority **pQueuePriority, PRIORITY priority) {
    if ((*pQueuePriority) == NULL) {
        (*pQueuePriority) = malloc(sizeof(patient_queue_priority));
    }

    patient_queue_priority *patient_queue = (*pQueuePriority);

    if (patient_queue->queue == NULL) {
        printf("pointer to queue is NULL: no patients in queue\n");
        /* set list head */
        patient_queue->queue = malloc(sizeof(patient));
        patient_queue->queue[0].priority = none;

        /* add new patient */
        patient *new = malloc(sizeof(patient));
        patient_queue->queue->next = new;
        patient_queue->queue->next->priority = priority;
        patient_queue->queue->next->is_active = (int) Random() % 2; //FIXME integrate with probability to be inactive and handle it

        /* increment patients */
        patient_queue->number++;
        return;
    }

    printf("pointer to queue is NOT NULL: there are patients in queue\n");
    patient *p_queue = patient_queue->queue;

    /* Add patient to list - ordering by priority */
    patient *new = malloc(sizeof(patient));
    new->priority = priority;
    new->is_active = (int) Random() % 2; //FIXME integrate with probability to be inactive and handle it

    patient *current = p_queue;
    while (current->priority <= new->priority && current->next!=NULL) {
        // Next in the list
        current = current->next;
    }
    current->next = new;

    /* Increment number of patients in queue */
    patient_queue->number++;
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

    if (policy == ABO_Comp) {
        /* handle matching with policy ABO-compatible */
    } else {
        /* handle matching with policy ABO-identical */
        handleMatchingABOIdentical(patient_qs[0], organ_qs[0], pWaitingList, bank); // O
        handleMatchingABOIdentical(patient_qs[1], organ_qs[1], pWaitingList, bank); // A
        handleMatchingABOIdentical(patient_qs[2], organ_qs[2], pWaitingList, bank); // B
        handleMatchingABOIdentical(patient_qs[3], organ_qs[3], pWaitingList, bank); // AB
    }
}

void initializeOrganQueue(organ_queue **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(organ_queue));
        (*pQueue)->bt = bloodType;
    }

    if ((*pQueue)->queue == NULL) {
        (*pQueue)->queue = malloc(sizeof(organ));
    }
}

void initializePatientQueue(patient_queue_blood_type **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(patient_queue_blood_type));
        (*pQueue)->bt = bloodType;
    }

    for (int i = 0; i < 3; ++i) {
        if ((*pQueue)->priority_queue[i] == NULL) {
            (*pQueue)->priority_queue[i] = malloc(sizeof(patient_queue_priority));
        }
        if ((*pQueue)->priority_queue[i]->queue == NULL) {
            (*pQueue)->priority_queue[i]->queue = malloc(sizeof(patient));
        }
        (*pQueue)->priority_queue[i]->priority = (PRIORITY)i+1; // skip default priority
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

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q) {

}


/***
 * UTILS
 */

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

double handle_matching() { return 0; }

double handle_transplant() { return 0; };