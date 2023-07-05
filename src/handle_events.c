#include "headers/utils.h"

/***
 * ------------------------------------------------ EVENT HANDLERS ----------------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */

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

    /* Set queue attributes */
    (*pQueue)->bt = bloodType;
    (*pQueue)->number = (*pQueue)->number+1;
    (*pQueue)->organ_available = true;

    /* Check if queue is empty */
    if ((*pQueue)->queue == NULL) {
        printf("pointer to queue is NULL: no organs in queue\n");
        (*pQueue)->queue = malloc(sizeof(organ));
        (*pQueue)->queue[0].starting_age = ((int) Random() % (24-0+1)) + 0; /* Starting age is between 0 and 24 h */
        return;
    }

    /* Add organ to list (not empty) */
    printf("pointer to queue is NOT NULL: there are organs in queue\n");
    organ *o_queue = (*pQueue)->queue;
    while (o_queue->next != NULL) {
        o_queue = o_queue->next;
    }
    o_queue->next = malloc(sizeof(organ));
    o_queue->next->starting_age = ((int) Random() % (24-0+1)) + 0;
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
            printf("Arrived patient with priority Critical\n");
            break;
        case normal:
            addPatientToPriorityQueue(&blood_queue->priority_queue[1], priority);
            malloc_handler(1, (void *[]) {blood_queue->priority_queue[1]});
            blood_queue->priority_queue[1]->priority = normal;
            printf("Arrived patient with priority Normal\n");
            break;
        case low:
            addPatientToPriorityQueue(&blood_queue->priority_queue[2], priority);
            malloc_handler(1, (void *[]) {blood_queue->priority_queue[2]});
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
    patient_queue->number = patient_queue->number + 1;
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
    bank->total_number = bank->total_number - 1;
    switch (bloodType) {
        case O:
            removeExpiredOrgans(bloodType, &bank->queues[0]);
            break;
        case A:
            removeExpiredOrgans(bloodType, &bank->queues[1]);
            break;
        case B:
            removeExpiredOrgans(bloodType, &bank->queues[2]);
            break;
        case AB:
            removeExpiredOrgans(bloodType, &bank->queues[3]);
            break;
        default:
            break;
    }
}

/***
 * Removes every expired organ from a specific blood type queue in the organ bank
 * @param bloodType
 * @param pQueue
 */
void removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue) {
    organ *o_queue = (*pQueue)->queue;
    organ *current, *prev, *tmp;

    current = o_queue;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
        if (current->starting_age >= 24) {
            /* The organ has expired */
            //FIXME add free
            prev->next = current->next;
            printf("An organ has expired in the queue with blood type %d\n", bloodType);
            /* Decrement organ number in queue */
            (*pQueue)->number = (*pQueue)->number - 1;
        }
    }

    /* check if no organs are left in queue */
    if ((*pQueue)->number == 0) {
        printf("No organs left in queue with blood type %d\n", bloodType);
        (*pQueue)->organ_available = false;
    }
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
    patient *next;
    patient *current = priority_queue;
    int max_number, index;
    int i=0;

    if (reason == death) {
        /* Handle patient death from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I remove the oldest one
        next = current->next->next;
        current->next = next;
        printf("A patient is dead in queue with priority %d\n", priority_queue->priority);
    } else {
        /* Handle patient renege from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I choose a random job
        max_number = (int) (*pQueuePriority)->number-1;
        index = ((int) rand() % (max_number-0+1)) + 0; // FIXME alternative to rand()
        while (i <= index && current->next != NULL) {
            current = current->next;
            i++;
        }

        next = current->next->next;
        current->next = next;
        printf("A patient left the queue with priority %d\n", priority_queue->priority);
    }

    /* Decrement number of patients in priority queue */
    (*pQueuePriority)->number = (*pQueuePriority)->number - 1;
}

double handle_transplant() { return 0; };
double handle_matching() { return 0; }