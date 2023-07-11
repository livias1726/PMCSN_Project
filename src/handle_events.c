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
void handleOrganArrival(BLOOD_TYPE bloodType, patient_waiting_list *wl, organ_bank *bank, transplant *transplantCenter) {

    /* new organ */
    organ *o = new_organ(bloodType);

    bool match = handleMatchingFromOrgan(bloodType, wl, transplantCenter, o);

    if (!match){
        addOrganToQueue(&bank->queues[bloodType], bank, o);
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
void addOrganToQueue(organ_queue **pQueue, organ_bank *bank, organ *o) {

    organ *curr;

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
    curr->next = o;
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
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list *waitingList, organ_bank *bank,
                          transplant *transplantCenter) {

    /* New patient */
    patient* p = new_patient(bloodType, priority);

    bool match = handleMatchingFromPatient(bloodType, bank, transplantCenter, p);

    if (!match) {
        patient_queue_blood_type **pbtQueue = &waitingList->blood_type_queues[bloodType];
        patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];
        addPatientToQueue(ppQueue, waitingList, *pbtQueue, p);

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
void addPatientToQueue(patient_queue_priority **pQueuePriority, patient_waiting_list *waitingList,
                       patient_queue_blood_type *queueBloodType, patient *p) {

    patient *curr;

    /* increment patients numbers */
    incrementPatients((*pQueuePriority), queueBloodType, waitingList);

    if ((*pQueuePriority)->number == 0.0) {
        printf("No patients in queue\n");
    } else {
        printf("There are patients in queue\n");
    }

    /* add new patient */
    GET_LAST_NODE((*pQueuePriority)->queue, curr)
    curr->next = p;
}


/***
 * ORGAN RENEGE
 */

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganRenege(BLOOD_TYPE bloodType, organ_bank *bank, organs_expired *expired) {
    int num_deleted = removeExpiredOrgans(bloodType, &bank->queues[bloodType], bank, expired);
    if (num_deleted != 0) bank->total_number -= num_deleted;
}

/***
 * Removes every expired organ from a specific blood type queue in the organ bank
 * @param bloodType
 * @param pQueue
 */
int removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue, organ_bank *bank, organs_expired *expiredQueue) {
    organ *o_queue = (*pQueue)->queue;
    organ *current;
    organ *o;
    int index = 0;
    int number_deleted = 0;

    current = o_queue;
    while (current->next != NULL) {
        current = current->next;
        if (current->starting_age >= 24) {
            /* The organ has expired */
            o = removeOrgan(index, pQueue, bank);
            if (o != NULL) {
                addOrganToLost(o, &expiredQueue);
                printf("An organ has expired in the queue with blood type %d\n", bloodType);
                number_deleted++;
            }
        }
        index++;
    }
    return number_deleted;
}

/***
 * PATIENT LOSS
 */
void handlePatientLoss(LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr, patient_waiting_list* wl, patients_lost *loss_queue) {
    patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bt];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[pr];
    patientLossInternal(reason, ppQueue, *pbtQueue, wl, loss_queue);
}

void patientLossInternal(LOSS_REASON reason, patient_queue_priority **pQueuePriority,
                         patient_queue_blood_type *queueBloodType, patient_waiting_list *waitingList,
                         patients_lost *lossQueue) {
    patient *priority_queue = (*pQueuePriority)->queue;
    patient *p;
    int max_number, index;
    max_number = (int) (*pQueuePriority)->number;

    if (reason == death) {
        /* Handle patient death from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I remove the oldest one
        index = 0;
    } else {
        /* Handle patient renege from priority queue */
        // TODO choose a random job or select a specific patient with id or remove oldest one
        // FIXME now I choose a random job
        index = ((int) rand() % (max_number-0+1)) + 0; // FIXME alternative to rand()
    }

    p = removePatient(index, pQueuePriority, queueBloodType, waitingList);
    if (p != NULL) {
        addPatientToLost(p, &lossQueue, reason);
        printf("A patient has left the queue with priority %d with reason %d\n", priority_queue->priority, reason);
    }
}

/***
 * -------------------------------------------------------------- MATCHING
 */

/***
 * This function is executed in each simulation run and checks for available matching possible between organs and
 * patients.
 * @param patient_q
 * @param organ_q
 * @param pList
 * @param bank
 * @param tc
 */
void handleMatchingInternal(patient_queue_blood_type *patient_q, organ_queue *organ_q, patient_waiting_list *pList,
                            organ_bank *bank, transplant *tc) {
    organ *o;
    patient *p;
    while (organ_q->organ_available && patient_q->number > 0) {
        // while there are available organs
        while (patient_q->priority_queue[critical]->number != 0 && organ_q->organ_available) {
            // while there are available critical patients
            o = removeOrgan(0, &organ_q, bank);
            p = removePatient(0, &patient_q->priority_queue[0], patient_q, pList);
            addMatchedToTransplant(o, p, &tc);
        }

        while (patient_q->priority_queue[normal]->number != 0 && organ_q->organ_available) {
            // while there are available normal patients
            o = removeOrgan(0, &organ_q, bank);
            p = removePatient(0, &patient_q->priority_queue[1], patient_q, pList);
            addMatchedToTransplant(o, p, &tc);
        }

        while (patient_q->priority_queue[low]->number != 0 && organ_q->organ_available) {
            // while there are available low patients
            o = removeOrgan(0, &organ_q, bank);
            p = removePatient(0, &patient_q->priority_queue[2], patient_q, pList);
            addMatchedToTransplant(o, p, &tc);
        }
    }
}

/**
 * Upon the arrival of a new patient, this function search for a compatible organ in queue.
 * If an organ is found, the patient is transplanted without inserting them in the waiting list and the organ is removed.
 *
 * //TODO: check if the patient can be transplanted due to their priority and other patients in queue!!!
 * */
bool handleMatchingFromPatient(BLOOD_TYPE bt, organ_bank *bank, transplant *tc, patient *patient) {

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

    handleTransplantFromPatient(avbOrganBt, bank, patient, tc);
    return found;
}

/**
 * Upon the arrival of a new organ, this function search for a compatible patient in queue to allocate the organ to.
 * If the patient is found, the organ is transplanted without inserting it in the organ bank and the patient is removed.
 * */
 // TODO: to be reviewed (ME FA SCHIFO MESSA COSI)
bool handleMatchingFromOrgan(BLOOD_TYPE bt, patient_waiting_list *wl, transplant *tc, organ *organ) {

    BLOOD_TYPE avbPatientBt;
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

    handleTransplantFromOrgan(avbPatientBt, avbPatientPr, wl, organ, tc);
    return found;
}

// ------------------------------------------------------- TO CHECK -----------------------------------
//TODO: this function and the other three can be merged in a single function if the allocation
// policy is used as a compilation flag
void handleMatching(patient_waiting_list *wl, organ_bank *bank, transplant *tc) {
    patient_queue_blood_type **patient_qs = wl->blood_type_queues;
    organ_queue **organ_qs = bank->queues;

#ifdef ABO_ID
    if (IDENTICAL((*organ_qs)->bt, (*patient_qs)->bt)) {
        handleMatchingInternal((*patient_qs), (*organ_qs), wl, bank, tc);
    }
#else
    if (COMPATIBLE((*organ_qs)->bt, (*patient_qs)->bt)) {
        handleMatchingInternal((*patient_qs), (*organ_qs), wl, bank, tc);
    }
#endif
}

/***
 * ------------------------------------------------- Transplant -----------------------------------------------------------
 */

void handleTransplantFromOrgan(BLOOD_TYPE bt, PRIORITY pr, patient_waiting_list *wl, organ *o, transplant *tc) {
    /* get first patient */
    patient *p = removePatient(0, &wl->blood_type_queues[bt]->priority_queue[pr], wl->blood_type_queues[bt], wl);
    addMatchedToTransplant(o, p, &tc);
}

void handleTransplantFromPatient(BLOOD_TYPE bt, organ_bank *bank, patient *p, transplant *tc) {
    /* get oldest organ */
    organ *o = removeOrgan(0, &bank->queues[bt], bank);
    addMatchedToTransplant(o, p, &tc);
}

void addMatchedToTransplant(organ *organ, patient *patient, transplant **tr_center) {
    matched *curr;
    matched *m = malloc(sizeof(matched));
    m->patient = *patient;
    m->organ = *organ;

    /* add to transplant center */
    /* roll to last node */
    GET_LAST_NODE((*tr_center)->matched_list, curr)

    /* append patient node */
    curr->next = m;

    /* increment transplant number */
    (*tr_center)->total_number++;
}


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

organ * removeOrgan(int idx, organ_queue **pQueue, organ_bank *bank) {

    organ *prev = NULL;
    organ *current = (*pQueue)->queue; //head
    organ *next = current->next; //first organ

    if ((*pQueue)->number == 0) {
        /* fixme: this event is impossible if there are no organs in queue */
        return NULL;
    }
    REMOVE_MID_NODE(idx, current, prev, next);
    current->next = NULL;
    decrementOrgans((*pQueue), bank);
    return current;
}

patient * removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                        patient_waiting_list *pList) {

    patient *prev = NULL;
    patient *current = (*pQueue)->queue;
    patient *next = current->next;

    if ((*pQueue)->number == 0) {
        /* fixme: this event is impossible if there are no patients in queue */
        return NULL;
    }
    REMOVE_MID_NODE(idx, current, prev, next);
    current->next = NULL;
    decrementPatients((*pQueue), pQueueBT, pList);
    return current;
}

void addPatientToLost(patient *p, patients_lost **pQueue, LOSS_REASON reason) {
    patient *curr;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;

    /* roll to last node */
    GET_LAST_NODE((*pQueue)->queue, curr)

    /* append patient node */
    curr->next = p;

    /* increment loss */
    if (reason == death) {
        (*pQueue)->number_dead[bt][pr]++;
    } else {
        (*pQueue)->number_renege[bt][pr]++;
    }
}

void addOrganToLost(organ *o, organs_expired **pQueue) {
    organ *curr;
    BLOOD_TYPE bt = o->bt;

    /* roll to last node */
    GET_LAST_NODE((*pQueue)->queue, curr)

    /* append organ node */
    curr->next = o;

    /* increment loss */
    (*pQueue)->number[bt]++;
}