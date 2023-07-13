#include "headers/utils.h"

/***
 * ------------------------------------------------ EVENT HANDLERS ----------------------------------------------------
 */

/***
 * ORGAN ARRIVAL
 */


void updateOffsets(in_activation *inactive);

/***
 * Function to handle the arrival of a new organ in a blood type queue of the organ bank
 * @param bloodType
 * @param bank
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType) {

    // Generate next arrival time of an organ
    events->organArrival.interArrivalTime[bloodType] = getOrganArrival(bloodType, t->current);
    t->last[0] = t->current;

    organ_bank *bank = &events->organArrival;

    /* new organ */
    organ *o = new_organ(bloodType);

    bool match = handleMatchingFromOrgan(events, t, bloodType, o);

    if (!match){
        addOrganToQueue(events, t, &bank->queues[bloodType], o);
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
void addOrganToQueue(event_list *events, sim_time *t, organ_queue **pQueue, organ *o) {

    organ *curr;
    organ_bank *bank = &events->organArrival;

    /* Increment organs */
    incrementOrgans((*pQueue), bank);

    /* add new organ */
    GET_LAST_NODE((*pQueue)->queue, curr)
    curr->next = o;

    /* Check if there is just one organ to eventually activate next organ renege */
    if ((*pQueue)->number == 1) {
        printf("Only one organ in queue - "
               "activating renege of organs with blood type %d\n", o->bt);
        events->organsLoss.renegingTime[o->bt] = getOrganRenege(o->bt, t->current);
    }
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

void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType, PRIORITY priority) {

    // Generate next arrival time of a patient
    events->patientArrival.interArrivalTime[bloodType][priority] = getPatientArrival(bloodType, priority, t->current);
    t->last[0] = t->current;

    /* New patient */
    patient* p = new_patient(bloodType, priority);

    /* Check if the patient has priority low - it means that has to get activated first! */
    if (priority == low) {
        addToActivationCenter(events, t, p);
        return;
    }

    addToWaitingList(events, t, p);
}

void addToWaitingList(event_list *events, sim_time* t, patient *p) {
    patient_waiting_list *wl = &events->patientArrival;
    BLOOD_TYPE bloodType = p->bt;
    PRIORITY priority = p->priority;

    bool match = handleMatchingFromPatient(events, t, bloodType, p);

    if (!match) {
        patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bloodType];
        patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];
        addPatientToQueue(events, t, ppQueue, *pbtQueue, p);

        printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
    } else {
        printf("Arrived and served patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
    }
}

/***
 * ------------ACTIVATION----------------
 */
in_activation *getNextActivation(double minCompletion, in_activation* inactive) {
    while (inactive->next != NULL) {
        inactive = inactive->next;
        if (inactive->completionTime == minCompletion) {
            return inactive;
        }
    }
}

void handlePatientActivation(event_list *events, sim_time *t) {
    in_activation *inactive = events->activationArrival.inactive_patients;
    double min = getMinActivation(inactive);
    in_activation *n = getNextActivation(min, inactive);

    int idx = n->serverOffset-1;
    in_activation *prev = NULL;
    in_activation *curr = inactive; //head
    in_activation *next = curr->next; //first inactive

    /*remove inactive patient */
    REMOVE_MID_NODE(idx, curr, prev, next)
    curr->next = NULL;
    events->activationArrival.total_number--;

    /* update offsets */
    updateOffsets(inactive);

    /* get patient to activate and send it to the waiting list with low priority */
    patient* p = n->patient;
    addToWaitingList(events, t, p);
    printf("Patient with blood type %d activated and sent to waiting list\n", p->bt);
}

void updateOffsets(in_activation *inactive) {
    int idx = 1;
    while (inactive->next != NULL) {
        inactive = inactive->next;
        inactive->serverOffset = idx;
        idx++;
    }
}

/***
 *
 * @param events
 * @param t
 * @param p
 */
void addToActivationCenter(event_list *events, sim_time *t, patient *p) {
    activation *ac = &events->activationArrival;
    in_activation *curr;

    /* increment number inactive */
    ac->total_number++;

    /* add patient to activation queue */
    in_activation *inactive = malloc(sizeof(in_activation));
    inactive->patient = p;
    inactive->next = NULL;
    inactive->serverOffset = (int)ac->total_number;

    /* generate and change activation time */
    inactive->completionTime = getActivationCompletion(t->current);

    /* add to activation servers */
    GET_LAST_NODE(ac->inactive_patients, curr);
    curr->next = inactive;
}

/***
 * Adds the patient to a specific queue with a specific priority
 * @param pQueuePriority
 * @param priority
 */
void addPatientToQueue(event_list *events, sim_time *t, patient_queue_priority **pQueuePriority,
                       patient_queue_blood_type *queueBloodType, patient *p) {

    patient *curr;
    patient_waiting_list *wl = &events->patientArrival;

    /* increment patients numbers */
    incrementPatients((*pQueuePriority), queueBloodType, wl);

    /* add new patient */
    GET_LAST_NODE((*pQueuePriority)->queue, curr)
    curr->next = p;

    /* Check if there is just one patient in queue to eventually activate patient death and renege */
    if ((*pQueuePriority)->number == 1) {
        printf("Only one patient in queue - "
               "activating patient death and renege with blood type %d and priority %d\n", p->bt, p->priority);
        events->patientsLoss.renegingTime[p->bt][p->priority] = getPatientRenege(p->bt, p->priority, t->current);
        events->patientsLoss.deathTime[p->bt][p->priority] = getPatientDeath(p->priority, p->bt, t->current);
    }
}


/***
 * ORGAN RENEGE
 */

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 * @param bloodType
 * @param pBank
 */
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bloodType) {
    
    // Generate next renege time for organ
    events->organsLoss.renegingTime[bloodType] = getOrganRenege(bloodType, t->current);
    t->last[0] = t->current;

    organ_bank *bank = &events->organArrival;
    organs_expired *expired = &events->organsLoss;
    
    /* Remove the oldest organ in the queue */
    organ *o = removeOrgan(0, bank->queues, bank);
    addOrganToLost(events, t, o, &expired);

    /*FIXME remove: int num_deleted = removeExpiredOrgans(bloodType, &bank->queues[bloodType], bank, expired);
     * if (num_deleted != 0) bank->total_number -= num_deleted;*/
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
                addOrganToLost(NULL, NULL, o, &expiredQueue);
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
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr) {

    patient_waiting_list *wl = &events->patientArrival;

    patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bt];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[pr];
    patientLossInternal(events, t, reason, ppQueue, *pbtQueue);
}

void patientLossInternal(event_list *events, sim_time *t, LOSS_REASON reason, patient_queue_priority **pQueuePriority,
                         patient_queue_blood_type *queueBloodType) {
    patient_waiting_list *wl = &events->patientArrival;
    patients_lost *lost = &events->patientsLoss;
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

    p = removePatient(index, pQueuePriority, queueBloodType, wl);
    if (p != NULL) {
        addPatientToLost(events, t, p, &lost, reason);
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
            addMatchedToTransplant(NULL, NULL, o, p);
        }

        while (patient_q->priority_queue[normal]->number != 0 && organ_q->organ_available) {
            // while there are available normal patients
            o = removeOrgan(0, &organ_q, bank);
            p = removePatient(0, &patient_q->priority_queue[1], patient_q, pList);
            addMatchedToTransplant(NULL, NULL, o, p);
        }

        while (patient_q->priority_queue[low]->number != 0 && organ_q->organ_available) {
            // while there are available low patients
            o = removeOrgan(0, &organ_q, bank);
            p = removePatient(0, &patient_q->priority_queue[2], patient_q, pList);
            addMatchedToTransplant(NULL, NULL, o, p);
        }
    }
}

/**
 * Upon the arrival of a new patient, this function search for a compatible organ in queue.
 * If an organ is found, the patient is transplanted without inserting them in the waiting list and the organ is removed.
 *
 * //TODO: check if the patient can be transplanted due to their priority and other patients in queue!!!
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *patient) {

    organ_bank *bank = &events->organArrival;

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

    handleTransplantFromPatient(events, t, avbOrganBt, patient);
    return found;
}

/**
 * Upon the arrival of a new organ, this function search for a compatible patient in queue to allocate the organ to.
 * If the patient is found, the organ is transplanted without inserting it in the organ bank and the patient is removed.
 * */
 // TODO: to be reviewed (ME FA SCHIFO MESSA COSI)
bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, organ *organ) {

    patient_waiting_list *wl = &events->patientArrival;

    BLOOD_TYPE avbPatientBt;
    PRIORITY avbPatientPr;
    bool found = false;

#ifdef ABO_ID
    if (wl->blood_type_queues[bt]->number == 0) return found; // no patient in list for a ABO_identical transplant
    found = true;
    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        if ((wl->blood_type_queues[bt]->priority_queue[i]->number != 0)) {
            avbPatientPr = i;
            avbPatientBt = bt;
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

    handleTransplantFromOrgan(events, t, avbPatientBt, avbPatientPr, organ);
    return found;
}

// ------------------------------------------------------- TO CHECK -----------------------------------
//TODO: this function and the other three can be merged in a single function if the allocation
// policy is used as a compilation flag
// FIXME REMOVE
void handleMatching(event_list *events) {

    patient_waiting_list *wl = &events->patientArrival;
    organ_bank *bank = &events->organArrival;
    transplant *transplant = &events->transplantArrival;

    patient_queue_blood_type **patient_qs = wl->blood_type_queues;
    organ_queue **organ_qs = bank->queues;

#ifdef ABO_ID
    if (IDENTICAL((*organ_qs)->bt, (*patient_qs)->bt)) {
        handleMatchingInternal((*patient_qs), (*organ_qs), wl, bank, transplant);
    }
#else
    if (COMPATIBLE((*organ_qs)->bt, (*patient_qs)->bt)) {
        handleMatchingInternal((*patient_qs), (*organ_qs), wl, bank, transplant);
    }
#endif
}

/***
 * ------------------------------------------------- Transplant -----------------------------------------------------------
 */

void handleTransplantFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr, organ *o) {

    patient_waiting_list *wl = &events->patientArrival;

    /* get first patient */
    patient *p = removePatient(0, &wl->blood_type_queues[bt]->priority_queue[pr], wl->blood_type_queues[bt], wl);
    addMatchedToTransplant(events, t, o, p);
}

void handleTransplantFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *p) {

    organ_bank *bank = &events->organArrival;

    /* get oldest organ */
    organ *o = removeOrgan(0, &bank->queues[bt], bank);
    addMatchedToTransplant(events, t, o, p);
}

void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient) {
    matched *curr;
    matched *m = malloc(sizeof(matched));
    m->patient = *patient;
    m->organ = *organ;

    patient_waiting_list *wl = &events->patientArrival;
    organ_bank *bank = &events->organArrival;
    transplant *tc = &events->transplantArrival;

    /* add to transplant center */
    /* roll to last node */
    GET_LAST_NODE(tc->matched_list, curr)

    /* append patient node */
    curr->next = m;

    /* increment transplant number */
    tc->total_number++;

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[patient->bt]->priority_queue[patient->priority]->number == 0) {
        printf("patient queue with blood type %d and priority %d is now empty -"
               "deactivating death and renege event\n", patient->bt, patient->priority);
        events->patientsLoss.renegingTime[patient->bt][patient->priority] = INFINITY;
        events->patientsLoss.deathTime[patient->bt][patient->priority] = INFINITY;
    } else {
        events->patientsLoss.renegingTime[patient->bt][patient->priority] = getPatientRenege(patient->bt, patient->priority, t->current);
        events->patientsLoss.deathTime[patient->bt][patient->priority] = getPatientDeath(patient->priority, patient->bt, t->current);
    }

    /* Check if organ queue is empty to eventually deactivate organ renege events */
    if (bank->queues[organ->bt]->number == 0) {
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", organ->bt);
        events->organsLoss.renegingTime[organ->bt] = INFINITY;
    } else {
        events->organsLoss.renegingTime[organ->bt] = getOrganRenege(organ->bt, t->current);
    }
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

void addPatientToLost(event_list *events, sim_time *t, patient *p, patients_lost **pQueue, LOSS_REASON reason) {
    patient *curr;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    patient_waiting_list *wl = &events->patientArrival;

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

    /* Check if there are other patients in queue to eventually deactivate reneging and death events */
    if (wl->blood_type_queues[p->bt]->priority_queue[p->priority]->number == 0) {
        printf("patient queue with blood type %d and priority %d is now empty -"
               "deactivating death and renege event\n", p->bt, p->priority);
        events->patientsLoss.renegingTime[p->bt][p->priority] = INFINITY;
        events->patientsLoss.deathTime[p->bt][p->priority] = INFINITY;
    } else {
        // generate next loss time (death and renege)
        events->patientsLoss.renegingTime[p->bt][p->priority] = getPatientRenege(p->bt, p->priority, t->current);
        events->patientsLoss.deathTime[p->bt][p->priority] = getPatientDeath(p->priority, p->bt, t->current);
    }
}

void addOrganToLost(event_list *events, sim_time *t, organ *o, organs_expired **pQueue) {
    organ *curr;
    BLOOD_TYPE bt = o->bt;
    organ_bank *bank = &events->organArrival;

    /* roll to last node */
    GET_LAST_NODE((*pQueue)->queue, curr)

    /* append organ node */
    curr->next = o;

    /* increment loss */
    (*pQueue)->number[bt]++;

    /* Check if the organ queue is empty to eventually deactivate loss event */
    if (bank->queues[o->bt]->number == 0) {
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", o->bt);
        events->organsLoss.renegingTime[o->bt] = INFINITY;
    } else {
        // generate next organ loss time
        events->organsLoss.renegingTime[o->bt] = getOrganRenege(o->bt, t->current);
    }
}