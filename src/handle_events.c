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
#ifdef AUDIT
        printf("Arrived organ with blood type %s\n", bt_to_str[bloodType]);
    } else {
        printf("Arrived and dispatched organ with blood type %s\n", bt_to_str[bloodType]);
#endif
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
#ifdef AUDIT
        printf("Only one organ in queue - "
               "activating renege of organs with blood type %d\n", o->bt);
#endif
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
#ifdef AUDIT
        printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
    } else {
        printf("Arrived and served patient with blood type %s and priority %s\n", bt_to_str[bloodType], prio_to_str[priority]);
#endif
    }
}

/***
 * ------------ACTIVATION----------------
 */
in_activation *getNextActivation(double minCompletion, in_activation* inactive) {
    while (inactive->next != NULL) {
        inactive = inactive->next;
        if (inactive->completionTime == minCompletion) return inactive;
    }

    return NULL;
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
    updateActivationOffsets(inactive);

    /* get patient to activate and send it to the waiting list with low priority */
    patient* p = n->patient;
    addToWaitingList(events, t, p);
#ifdef AUDIT
    printf("Patient with blood type %d activated and sent to waiting list\n", p->bt);
#endif
}

void updateActivationOffsets(in_activation *inactive) {
    int idx = 1;
    while (inactive->next != NULL) {
        inactive = inactive->next;
        inactive->serverOffset = idx;
        idx++;
    }
}

/***
 * Adds inactive patient to the activation center
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
    GET_LAST_NODE(ac->inactive_patients, curr)
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
#ifdef AUDIT
        printf("Only one patient in queue - "
               "activating patient death and renege with blood type %d and priority %d\n", p->bt, p->priority);
#endif
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
#ifdef AUDIT
        printf("A patient has left the queue with priority %d with reason %d\n", priority_queue->priority, reason);
#endif
    }
}

/***
 * -------------------------------------------------------------- MATCHING
 */

/**
 * Upon the arrival of a new patient, this function search for a compatible organ in queue.
 * If an organ is found, the patient is transplanted without inserting them in the waiting list and the organ is removed.
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *patient) {
    PRIORITY pr = patient->priority;
    PRIORITY priority_placement = pr;
    patient_queue_priority **pQueue = events->patientArrival.blood_type_queues[bt]->priority_queue;

    /* check if the patient can be transplanted due to their priority and other patients in queue */
    switch (pr) {
        case critical:
            // instantly serve patient
            break;
        case normal:
            // if there are critical patients then serve patient in queue critical
            if (pQueue[critical]->number != 0) priority_placement = critical;
            break;
        case low:
            if (pQueue[critical]->number != 0) { // check if there are critical patients
                // serve patient in queue critical
                priority_placement = critical;
            } else if (pQueue[normal]->number != 0) { // check if there are normal patients
                // serve patient in queue normal
                priority_placement = normal;
            }
            break;
        case none:
            break;
    }

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

    if (priority_placement == pr) {
        // serve original patient
    } else {
        // serve new selected patient and add original patient in queue
        addPatientToQueue(events, t,
                          &events->patientArrival.blood_type_queues[bt]->priority_queue[pr],
                          events->patientArrival.blood_type_queues[bt],
                          patient);
        patient = removePatient(0, &events->patientArrival.blood_type_queues[bt]->priority_queue[priority_placement],
                                events->patientArrival.blood_type_queues[bt],
                                &events->patientArrival);
    }

    handleTransplantFromPatient(events, t, avbOrganBt, patient);
    return found;
}

/**
 * Upon the arrival of a new organ, this function search for a compatible patient in queue to allocate the organ to.
 * If the patient is found, the organ is transplanted without inserting it in the organ bank and the patient is removed.
 * */
bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, organ *organ) {

    patient_waiting_list *wl = &events->patientArrival;

    BLOOD_TYPE avbPatientBt = nbt;
    PRIORITY avbPatientPr = none;
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

    int i = 0;
    while (i < NUM_PRIORITIES && !found) {
        for (int j = 0; j < size; ++j) {
            if ((wl->blood_type_queues[comp[j]]->priority_queue[i]->number != 0)) {
                avbPatientBt = comp[j];
                avbPatientPr = i;
                found = true;
                break;
            }
        }
        ++i;
    }
    if(!found) return found;
#endif

    handleTransplantFromOrgan(events, t, avbPatientBt, avbPatientPr, organ);
    return found;
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
    in_transplant *curr;

    patient_waiting_list *wl = &events->patientArrival;
    organ_bank *bank = &events->organArrival;
    transplant *tc = &events->transplantArrival;

    /* new match */
    matched *m = malloc(sizeof(matched));
    m->patient = *patient;
    m->organ = *organ;
    m->next = NULL;

    /* increment transplant number */
    tc->total_number++;

    /* add match to transplant queue */
    in_transplant *in_tr = malloc(sizeof(in_transplant));
    in_tr->matched = m;
    in_tr->next = NULL;
    in_tr->serverOffset = (int)tc->total_number;

    /* generate and change transplant completion time */
    in_tr->completionTime = getTransplantCompletion(t->current);

    /* add new in transplant */
    GET_LAST_NODE(tc->transplanted_patients, curr)
    curr->next = in_tr;

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[patient->bt]->priority_queue[patient->priority]->number == 0) {
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty -"
               "deactivating death and renege event\n", patient->bt, patient->priority);
#endif
        events->patientsLoss.renegingTime[patient->bt][patient->priority] = INFINITY;
        events->patientsLoss.deathTime[patient->bt][patient->priority] = INFINITY;
    } else {
        events->patientsLoss.renegingTime[patient->bt][patient->priority] = getPatientRenege(patient->bt, patient->priority, t->current);
        events->patientsLoss.deathTime[patient->bt][patient->priority] = getPatientDeath(patient->priority, patient->bt, t->current);
    }

    /* Check if organ queue is empty to eventually deactivate organ renege events */
    if (bank->queues[organ->bt]->number == 0) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", organ->bt);
#endif
        events->organsLoss.renegingTime[organ->bt] = INFINITY;
    } else {
        events->organsLoss.renegingTime[organ->bt] = getOrganRenege(organ->bt, t->current);
    }
}

in_transplant *getNextTransplant(double minCompletion, in_transplant *transplanted) {
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        if (transplanted->completionTime == minCompletion) {
            return transplanted;
        }
    }
    return NULL;
}

void updateTransplantOffsets(in_transplant *transplanted) {
    int idx = 1;
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        transplanted->serverOffset = idx;
        idx++;
    }
}

void handleTransplantCompletion(event_list *events, sim_time *t) {
    in_transplant *transplanted = events->transplantArrival.transplanted_patients;
    double min = getMinTransplant(transplanted);
    in_transplant *n = getNextTransplant(min, transplanted);

    int idx = n->serverOffset-1;
    in_transplant *prev = NULL;
    in_transplant *curr = transplanted; //head
    in_transplant *next = curr->next; //first inactive

    /*remove completed transplant */
    REMOVE_MID_NODE(idx, curr, prev, next)
    curr->next = NULL;
    events->transplantArrival.total_number--;

    /* update offsets */
    updateTransplantOffsets(transplanted);

    /* reject transplant with a probability below level and send patient back in queue with high priority */
    double prob = getRejectionProb();
    if (prob < REJECT_P) {
        patient *p = &n->matched->patient;
        p->priority = critical;
        addToWaitingList(events, t, p);
#ifdef AUDIT
        printf("Patient with blood type %d was rejected and and sent back to waiting list with priority %d\n", p->bt, p->priority);
#endif
        events->transplantArrival.rejected_transplants++;
    } else {
        events->transplantArrival.completed_transplants++;
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
#ifdef AUDIT
        printf("No organs left in queue with blood type %s\n", bt_to_str[organQueue->bt]);
#endif
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

    REMOVE_MID_NODE(idx, current, prev, next)
    current->next = NULL;
    decrementOrgans((*pQueue), bank);
    return current;
}

patient * removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                        patient_waiting_list *pList) {

    patient *prev = NULL;
    patient *current = (*pQueue)->queue;
    patient *next = current->next;

    REMOVE_MID_NODE(idx, current, prev, next)
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
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty -"
               "deactivating death and renege event\n", p->bt, p->priority);
#endif
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
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", o->bt);
#endif
        events->organsLoss.renegingTime[o->bt] = INFINITY;
    } else {
        // generate next organ loss time
        events->organsLoss.renegingTime[o->bt] = getOrganRenege(o->bt, t->current);
    }
}