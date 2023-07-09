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
void handleOrganArrival(BLOOD_TYPE bloodType, patient_waiting_list *wl, organ_bank *bank) {

    bool match = handleMatchingFromOrgan(bloodType, wl);

    if (!match){
        addOrganToQueue(&bank->queues[bloodType], bank);
        printf("Arrived organ with blood type %s\n", bt_to_str[bloodType]);
    } else {
        printf("Arrived and dispatched organ with blood type %s\n", bt_to_str[bloodType]);
    }
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
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list *waitingList,
                          organ_bank* bank) {

    bool match = handleMatchingFromPatient(bloodType, priority, waitingList, bank);

    if (!match) {
        patient_queue_blood_type **pbtQueue = &waitingList->blood_type_queues[bloodType];
        patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];
        addPatientToPriorityQueue(ppQueue, priority, waitingList,*pbtQueue);

        printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
    } else {
        printf("Arrived and served patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
    }
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
    int num_deleted = removeExpiredOrgans(bloodType, &bank->queues[bloodType], bank);
    if (num_deleted != 0) bank->total_number -= num_deleted;
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
 // TODO: these handlers can be merged into a single function that takes the loss_reason as parameter
void handlePatientDeath(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list) {
    list->total_number--;
    LOSS_REASON reason = death;

    //removePatientsFromBloodQueue(reason, priority, &list->blood_type_queues[bloodType], NULL);
    patient_queue_blood_type **pbtQueue = &list->blood_type_queues[bloodType];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];

    removePatientsFromPriorityQueue(reason, ppQueue, *pbtQueue, list);
}

/***
 * PATIENT RENEGE
 */
void handlePatientRenege(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* waitingList) {
    waitingList->total_number--;
    LOSS_REASON reason = renege;

    //removePatientsFromBloodQueue(reason, priority, &waitingList->blood_type_queues[bloodType], waitingList);
    patient_queue_blood_type **pbtQueue = &waitingList->blood_type_queues[bloodType];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];

    removePatientsFromPriorityQueue(reason, ppQueue, *pbtQueue, waitingList);
}


void removePatientsFromBloodQueue(LOSS_REASON reason, PRIORITY priority, patient_queue_blood_type **pQueue,
                                  patient_waiting_list *patientWaitingList) {
    /*
    patient_queue_blood_type *blood_queue = (*pQueue);
    removePatientsFromPriorityQueue(reason, &blood_queue->priority_queue[priority], blood_queue, patientWaitingList);
     */
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
 * -------------------------------------------------------------- MATCHING
 */

void handleMatchingInternal(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                            organ_bank *bank){
    while (organ_q->organ_available && patient_q->number > 0) {
        // fino a che ci sono organi disponibili
        while (patient_q->priority_queue[critical]->number != 0 && organ_q->organ_available) {
            // fino a che ci sono pazienti critici
            removeOrgan(0, &organ_q, bank);
            removePatient(0, &patient_q->priority_queue[0], patient_q, pList);
        }

        while (patient_q->priority_queue[normal]->number != 0 && organ_q->organ_available) {
            // fino a che ci sono pazienti normali
            removeOrgan(0, &organ_q, bank);
            removePatient(0, &patient_q->priority_queue[1], patient_q, pList);
        }

        while (patient_q->priority_queue[low]->number != 0 && organ_q->organ_available) {
            // fino a che ci sono pazienti low
            removeOrgan(0, &organ_q, bank);
            removePatient(0, &patient_q->priority_queue[2], patient_q, pList);
        }
    }
}

/**
 * Upon the arrival of a new patient, this function search for a compatible organ in queue.
 * If an organ is found, the patient is transplanted without inserting them in the waiting list and the organ is removed.
 *
 * //TODO: check if the patient can be transplanted due to their priority and other patients in queue!!!
 * */
bool handleMatchingFromPatient(BLOOD_TYPE bt, PRIORITY pr, patient_waiting_list *wl, organ_bank* bank) {

    bool found = false;
    BLOOD_TYPE avbOrganBt = bt;
    organ_queue **organQueues = bank->queues;

#ifdef ABO_ID
    if (organQueues[bt]->number == 0) return found;
    found = true;
#else

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if ((organQueues[i]->number != 0) && (COMPATIBLE(i, bt))) {
            avbOrganBt = i;
            found = true;
            break;
        }
    }

    if (!found) return found;
#endif

    handleTransplantFromPatient(avbOrganBt, bank);
    return found;
}

/**
 * Upon the arrival of a new organ, this function search for a compatible patient in queue to allocate the organ to.
 * If the patient is found, the organ is transplanted without inserting it in the organ bank and the patient is removed.
 * */
 // TODO: to be reviewed (ME FA SCHIFO MESSA COSI)
bool handleMatchingFromOrgan(BLOOD_TYPE bt, patient_waiting_list *wl) {

    BLOOD_TYPE avbPatientBt = bt;
    PRIORITY avbPatientPr;
    bool found = false;

#ifdef ABO_ID
    if (wl->blood_type_queues[bt]->number == 0) return found; // no patient in list for a ABO_identical transplant
    found = true;
    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        if ((wl->blood_type_queues[bt]->priority_queue[i]->number != 0)) {
            avbPatientPr = i;
            break;
        }
    }
#else
    const BLOOD_TYPE *comp = get_compatible_patient[bt];
    const int size = get_num_compatible_patients[bt];

    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        for (int j = 0; j < size; ++j) {
            if ((wl->blood_type_queues[comp[j]]->priority_queue[i]->number != 0)) {
                avbPatientBt = comp[j];
                avbPatientPr = i;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    if(!found) return found;
#endif

    handleTransplantFromOrgan(avbPatientBt, avbPatientPr, wl);
    return found;
}

// ------------------------------------------------------- TO CHECK -----------------------------------
//TODO: this function and the other three can be merged in a single function if the allocation
// policy is used as a compilation flag
void handleMatching(POLICY policy, patient_waiting_list *pWaitingList, organ_bank *bank) {
    patient_queue_blood_type **patient_qs = pWaitingList->blood_type_queues;
    organ_queue **organ_qs = bank->queues;

    //TODO: I don't think this will ever happen when queues are allocated at initialization time
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if (patient_qs[i] == NULL) {
            initializePatientQueue(&patient_qs[i], (BLOOD_TYPE)i);
        }
        if (organ_qs[i] == NULL) {
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

//TODO: is it necessary to check for compatibility if the calls have been filtered in the switch?
void handleMatchingABOIdentical(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                organ_bank *bank) {
    if (IDENTICAL(organ_q->bt, patient_q->bt)) {
        handleMatchingInternal(patient_q, organ_q, pList, bank);
    }
}

void handleMatchingABOCompatible(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                                 organ_bank *bank) {
    if (COMPATIBLE(organ_q->bt, patient_q->bt)) {
        handleMatchingInternal(patient_q, organ_q, pList, bank);
    }
}

/***
 * ------------------------------------------------- Transplant -----------------------------------------------------------
 */

void handleTransplantFromOrgan(BLOOD_TYPE bt, PRIORITY pr, patient_waiting_list *wl){ //TODO: temporary DUMMY transplant handling

    removePatient(0, &wl->blood_type_queues[bt]->priority_queue[pr], wl->blood_type_queues[bt], wl);
}

void handleTransplantFromPatient(BLOOD_TYPE bt, organ_bank *bank){ //TODO: temporary DUMMY transplant handling

    removeOrgan(0, &bank->queues[bt], bank);
}

double handle_transplant() { return 0; }

/***
 * ------------------------------------------------- UTILS -----------------------------------------------------------
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
        printf("No organs left in queue with blood type %s\n", bt_to_str[organQueue->bt]);
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

    organ *prev = NULL;
    organ *current = (*pQueue)->queue; //head
    organ *next = current->next; //first organ

    REMOVE_MID_NODE(idx, current, prev, next);

    decrementOrgans((*pQueue), bank);
}

void removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                   patient_waiting_list *pList) {

    patient *prev = NULL;
    patient *current = (*pQueue)->queue;
    patient *next = current->next;

    int i = 0;
    while (i < idx+1) {
        prev = current;
        current = next;
        next = current->next;
        i++;
    }
    prev->next = next;
    free(current);
    //REMOVE_MID_NODE(idx, current, prev, next);

    decrementPatients((*pQueue), pQueueBT, pList);
}