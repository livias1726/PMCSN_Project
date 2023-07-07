#include "headers/utils.h"

/***
 * ------------------------------------------------ EVENT HANDLERS ----------------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                 organ_bank *bank);


/***
 * Function to handle the arrival of a new organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank *bank) {
    addOrganToQueue(&bank->queues[bloodType], bank);
    printf("Arrived organ with blood type %s\n", bt_to_str[bloodType]);
}

/***
 * Adds an organ to the organ queue with a specific blood type
 * @param pQueue
 * @param bloodType
 */
void addOrganToQueue(organ_queue **pQueue, organ_bank *bank) {

    organ *curr, *new;

    /* Increment organs */
    incrementOrgans((*pQueue), bank);

    /* Check if queue is empty */
    if ((*pQueue)->number == 0.0) {
        printf("No organs in queue\n");
    } else {
        printf("There are organs in queue\n");
    }

    /* add new organ */
    GET_LAST_NODE((*pQueue)->queue, curr)

    new = malloc(sizeof(organ));
    MALLOC_HANDLER(new)
    new->starting_age = ((int) rand() % (24-0+1)) + 0;
    new->next = NULL;

    curr->next = new;
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
    addPatientToBloodQueue(&waitingList->blood_type_queues[bloodType], priority, waitingList);
    printf("Arrived patient with blood type %s\n", bt_to_str[bloodType]);
}


/***
 * Adds the patient to a specific queue with a specific blood type
 * @param pQueue
 * @param priority
 */
void addPatientToBloodQueue(patient_queue_blood_type **pQueue, PRIORITY priority, patient_waiting_list *waitingList) {

    addPatientToPriorityQueue(&(*pQueue)->priority_queue[priority], priority, waitingList,(*pQueue));
    printf("Arrived patient with priority %s\n", prio_to_str[priority]);
}

/***
 * Adds the patient to a specific queue with a specific priority
 * @param pQueuePriority
 * @param priority
 */
void addPatientToPriorityQueue(patient_queue_priority **pQueuePriority, PRIORITY priority,
                               patient_waiting_list *waitingList, patient_queue_blood_type *queueBloodType) {

    patient *curr, *new;

    /* increment patients numbers */
    incrementPatients((*pQueuePriority), queueBloodType, waitingList);

    if ((*pQueuePriority)->number == 0.0) {
        printf("No patients in queue\n");
    } else {
        printf("There are patients in queue\n");
    }

    /* add new patient */
    GET_LAST_NODE((*pQueuePriority)->queue, curr)

    new = malloc(sizeof(patient));
    MALLOC_HANDLER(new)
    new->priority = priority;
    new->is_active = (int) Random() % 2; // FIXME integrate with probability to be inactive and handle it
    new->next = NULL;

    curr->next = new;
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
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[0], bank);
            break;
        case A:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[1], bank);
            break;
        case B:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[2], bank);
            break;
        case AB:
            num_deleted = removeExpiredOrgans(bloodType, &bank->queues[3], bank);
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
int removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue, organ_bank *bank) {
    organ *o_queue = (*pQueue)->queue;
    organ *current;
    int index, number_deleted = 0;

    current = o_queue;
    while (current->next != NULL) {
        current = current->next;
        if (current->starting_age >= 24) {
            /* The organ has expired */
            removeOrgan(index, pQueue, bank);
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
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[0], NULL);
            break;
        case A:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[1], NULL);
            break;
        case B:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[2], NULL);
            break;
        case AB:
            removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[3], NULL);
            break;
        default:
            break;
    }
}

/***
 * PATIENT RENEGE
 */
void handlePatientRenege(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* waitingList) {
    waitingList->total_number = waitingList->total_number - 1;
    LOSS_REASON reason = renege;
    switch (bloodType) {
        case O:
            removePatientsFromBloodQueue(reason, priority, &waitingList->blood_type_queues[0], waitingList);
            break;
        case A:
            removePatientsFromBloodQueue(reason, priority, &waitingList->blood_type_queues[1], waitingList);
            break;
        case B:
            removePatientsFromBloodQueue(reason, priority, &waitingList->blood_type_queues[2], waitingList);
            break;
        case AB:
            removePatientsFromBloodQueue(reason, priority, &waitingList->blood_type_queues[3], waitingList);
            break;
        default:
            break;
    }
}


void removePatientsFromBloodQueue(LOSS_REASON reason, PRIORITY priority, patient_queue_blood_type **pQueue,
                                  patient_waiting_list *patientWaitingList) {
    patient_queue_blood_type *blood_queue = (*pQueue);

    switch (priority) {
        case critical:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[0], blood_queue, patientWaitingList);
            break;
        case normal:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[1], blood_queue, patientWaitingList);
            break;
        case low:
            removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[2], blood_queue, patientWaitingList);
            break;
        default:
            break;
    }
}

void removePatientsFromPriorityQueue(LOSS_REASON reason, patient_queue_priority **pQueuePriority,
                                     patient_queue_blood_type *queueBloodType, patient_waiting_list *waitingList) {
    patient *priority_queue = (*pQueuePriority)->queue;
    int max_number, index;

    if (reason == death) {
        /* Handle patient death from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I remove the oldest one
        removePatient(0, pQueuePriority, queueBloodType, waitingList);
        printf("A patient is dead in queue with priority %d\n", priority_queue->priority);
    } else {
        /* Handle patient renege from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I choose a random job
        max_number = (int) (*pQueuePriority)->number-1;
        index = ((int) rand() % (max_number-0+1)) + 0; // FIXME alternative to rand()
        removePatient(index, pQueuePriority, queueBloodType, waitingList);
        printf("A patient left the queue with priority %d\n", priority_queue->priority);
    }
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
                organ o = removeOrgan(0, &organ_q, bank);
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

organ removeOrgan(int idx, organ_queue **pQueue, organ_bank *bank) {
    int i = 0;
    organ *organs = (*pQueue)->queue;
    organ *current = organs;
    organ *prev = current;
    organ *next = current->next;

    while (i < idx+1) {
        prev = current;
        current = current->next;
        next = current->next;
        i++;
    }

    prev->next = next;
    decrementOrgans((*pQueue), bank);
    /* detach removed organ from the other organs in queue */
    current->next = NULL;
    return *current;
}

patient removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                   patient_waiting_list *pList) {
    int i = 0;
    patient *patients = (*pQueue)->queue;
    patient *current = patients;
    patient *prev = current;
    patient *next = current->next;

    while (i < idx+1) {
        prev = current;
        current = current->next;
        next = current->next;
        i++;
    }

    prev->next = next;
    decrementPatients((*pQueue), pQueueBT, pList);
    return *current;
}

double handle_transplant() { return 0; };