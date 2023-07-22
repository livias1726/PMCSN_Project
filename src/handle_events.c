#include "headers/utils.h"

/***
 * This module contains the implementation of every EVENT HANDLER for the system ---------------------------------------
 */

// Internal MATCHING
bool handleMatchingFromOrgan(event_list *, sim_time *, BLOOD_TYPE, organ *, bool living);
bool handleMatchingFromPatient(event_list*, sim_time*, BLOOD_TYPE, patient*);


// ------------------------------------------ ORGAN ARRIVAL -----------------------------------------------------------

patient *getOldestPatient(const BLOOD_TYPE *pBt, const int size, patient_waiting_list *wl);

// Internal usage: adds an organ to the organ queue with a specific blood type
void addOrganToQueue(event_list *events, sim_time *t, organ_queue **pQueue, organ *o) {

    organ *curr;
    organ_bank *bank = &events->organ_arrival;

    /* Increment organs */
    (*pQueue)->number++;
    if (!(*pQueue)->organ_available) {
        (*pQueue)->organ_available = true;
    }
    bank->total_number++;

    /* Add new organ */
    GET_LAST_NODE((*pQueue)->queue, curr)
    curr->next = o;

    /* Check if there is just one organ to eventually activate next organ renege */
    if ((*pQueue)->number == 1) {
#ifdef AUDIT
        printf("Only one organ in queue - activating renege of organs with blood type %d\n", o->bt);
#endif
        events->organs_loss.reneging_time[o->bt] = getOrganRenege(o->bt, t->current);
    }
}

/***
 * Handler of a new organ's arrival:
 *      - Checks the availability of a compatible patient in waiting list and eventually triggers a matching event
 *      - If no compatible patient is available, adds the organ in the specific blood type queue of the organ bank
 *
 * @param events
 * @param t
 * @param bloodType
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType, bool living) {

    organ *o;

    /* Generate next arrival time of an organ */
    if (living) {
        /* living donor organ */
        events->living_donor_completion[bloodType] = getLivingDonorOrganArrival(bloodType, t->current);
        o = newOrgan(bloodType, living);
    } else {
        /* deceased donor organ */
        events->organ_arrival.inter_arrival_time[bloodType] = getDecDonorOrganArrival(bloodType, t->current);
        o = newOrgan(bloodType, deceased);
    }
    t->last[organ_arrival] = t->current;

    /* increment arrivals */
    events->organ_arrival.num_arrivals[bloodType]++;

    /* Check for matching */
    bool match = handleMatchingFromOrgan(events, t, bloodType, o, living);

    if (!match && !living){
        /* Add new organ to the bank */
        organ_bank *bank = &events->organ_arrival;
        addOrganToQueue(events, t, &bank->queues[bloodType], o);
#ifdef AUDIT
        printf("Arrived organ with blood type %s\n", bt_to_str[bloodType]);
    } else {
        printf("Arrived and dispatched organ with blood type %s\n", bt_to_str[bloodType]);
#endif
    }
}

// ------------------------------------------------- PATIENT ARRIVAL ------------------------------------------------

// Internal usage: adds the patient to a specific queue with a specific priority
void addPatientToQueue(event_list *events, sim_time *t, patient_queue_priority **pQueuePriority,
                       patient_queue_blood_type *queueBloodType, patient *p) {

    patient *curr;
    patient_waiting_list *wl = &events->patient_arrival;

    /* Increment patients numbers */
    (*pQueuePriority)->number++;
    queueBloodType->number++;
    wl->total_number++;
    if (!queueBloodType->patient_available) queueBloodType->patient_available = true;

    /* increment arrivals */
    events->patient_arrival.num_arrivals[p->bt][p->priority]++;

    /* set patient arrival time*/
    p->start_time = t->current;

    /* add new patient */
    GET_LAST_NODE((*pQueuePriority)->queue, curr)
    curr->next = p;

    /* Check if there is just one patient in queue to eventually activate patient death and renege */
    if ((*pQueuePriority)->number == 1) {
        events->patients_loss.reneging_time[p->bt][p->priority] = getPatientRenege(p->bt, p->priority, t->current);
        events->patients_loss.death_time[p->bt][p->priority] = getPatientDeath(p->priority, p->bt, t->current);
#ifdef AUDIT
        printf("Only one patient in queue - activating patient death and renege with blood type %d and priority %d\n",
               p->bt, p->priority);
#endif
    }

    if (queueBloodType->number == 1) {
        events->living_donor_completion[p->bt] = getLivingDonorOrganArrival(p->bt, t->current);
#ifdef AUDIT
        printf("Only one patient in blood type queue - activating living donor arrival with blood type %d\n", p->bt);
#endif
    }
}

// Internal usage: adds the patient to a specific queue with a specific priority
void addToWaitingList(event_list *events, sim_time* t, patient *p) {
    patient_waiting_list *wl = &events->patient_arrival;
    BLOOD_TYPE bloodType = p->bt;
    PRIORITY priority = p->priority;

    bool match = handleMatchingFromPatient(events, t, bloodType, p);

    if (!match) {
        patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bloodType];
        patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];
        addPatientToQueue(events, t, ppQueue, *pbtQueue, p);
#ifdef AUDIT
        printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bloodType], pr_to_str[priority]);
    } else {
        printf("Arrived and served patient with blood type %s and priority %s\n", bt_to_str[bloodType], pr_to_str[priority]);
#endif
    }
    /*
    patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bloodType];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[priority];
    addPatientToQueue(events, t, ppQueue, *pbtQueue, p);*/
#ifdef AUDIT
    printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bloodType], pr_to_str[priority]);
#endif
}

// Internal usage: adds inactive patient to the activation_center center
void addToActivationCenter(event_list *events, sim_time *t, patient *p) {
    activation_center *ac = &events->activation_arrival;
    in_activation *curr;

    /* increment number inactive */
    ac->total_number++;

    /* add patient to activation_center queue */
    in_activation *inactive = newInactive(p, ac->total_number);

    /* generate and change activation_center time */
    inactive->completionTime = getActivationCompletion(t->current);

    /* add to activation_center servers */
    GET_LAST_NODE(ac->inactive_patients, curr)
    curr->next = inactive;
}

/***
 * Handler of a new patient's arrival:
 *      - Manages the event list according to the event of a patient's arrival
 *      - If the patient has low priority, they are added as 'inactive' to the activation_center center
 *      - Else, checks the availability of a compatible organ in the bank and eventually triggers a matching event
 *      - If no compatible organ is available, adds the patient in the waiting list
 *
 * @param events
 * @param t
 * @param bloodType
 * @param priority
 */
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bloodType, PRIORITY priority) {

    // Generate next arrival time of a patient
    events->patient_arrival.inter_arrival_time[bloodType][priority] = getPatientArrival(bloodType, priority, t->current);
    t->last[patient_arrival] = t->current;

    /* New patient */
    patient* p = newPatient(bloodType, priority);

    /* Check if the patient has priority low - it means that has to get activated first! */
    if (priority == low) {
        addToActivationCenter(events, t, p);
    } else {
        addToWaitingList(events, t, p);
    }
}

// -------------------------------------------------- ACTIVATION----------------------------------------------------

in_activation *getNextActivation(double minCompletion, in_activation* inactive) {
    while (inactive->next != NULL) {
        inactive = inactive->next;
        if (inactive->completionTime == minCompletion) return inactive;
    }

    return NULL;
}

void updateActivationOffsets(in_activation *inactive) {
    int idx = 1;
    while (inactive->next != NULL) {
        inactive = inactive->next;
        inactive->serverOffset = idx;
        idx++;
    }
}

/**
 * This event triggers the addition of an inactive patient (waiting in the activation_center center) to the waiting list
 * to wait for a transplant.
 * */
void handlePatientActivation(event_list *events, sim_time *t) {
    in_activation *inactive = events->activation_arrival.inactive_patients;
    double min = getMinActivation(inactive);
    in_activation *n = getNextActivation(min, inactive);

    t->last[activation] = t->current;

    int idx = (int)n->serverOffset-1;
    in_activation *prev = NULL;
    in_activation *curr = inactive; //head
    in_activation *next = curr->next; //first inactive

    /*remove inactive patient */
    REMOVE_MID_NODE(idx, curr, prev, next)
    curr->next = NULL;
    //free(curr);
    events->activation_arrival.total_number--;

    /* update offsets */
    updateActivationOffsets(inactive);

    /* get patient to activate and send it to the waiting list with low priority */
    patient* p = n->patient;
    addToWaitingList(events, t, p);
#ifdef AUDIT
    printf("Patient with blood type %d activated and sent to waiting list\n", p->bt);
#endif
}

// --------------------------------------------------- ORGAN RENEGE ---------------------------------------------------

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

organ * removeOrgan(int idx, organ_queue **pQueue, organ_bank *bank) {

    organ *prev = NULL;
    organ *current = (*pQueue)->queue; //head
    organ *next = current->next; //first organ

    REMOVE_MID_NODE(idx, current, prev, next)
    current->next = NULL;
    decrementOrgans((*pQueue), bank);
    return current;
}

void addOrganToLost(event_list *events, sim_time *t, organ *o, organs_expired **pQueue) {
    //organ *curr;
    BLOOD_TYPE bt = o->bt;
    organ_bank *bank = &events->organ_arrival;

    /* roll to last node */
    //GET_LAST_NODE((*pQueue)->queue, curr)

    /* append organ node */
    //curr->next = o;

    /* increment loss */
    (*pQueue)->number[bt]++;

    /* Check if the organ queue is empty to eventually deactivate loss event */
    if (!bank->queues[o->bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty - deactivating renege event\n", o->bt);
#endif
        events->organs_loss.reneging_time[o->bt] = INFINITY;
    } else {
        // generate next organ loss time
        events->organs_loss.reneging_time[o->bt] = getOrganRenege(o->bt, t->current);
    }
}

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 * @param bloodType
 * @param pBank
 */
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bloodType) {

    organ_bank *bank = &events->organ_arrival;
    organs_expired *expired = &events->organs_loss;
    
    /* Remove the oldest organ in the queue */
    organ *o = removeOrgan(0, &bank->queues[bloodType], bank);
    addOrganToLost(events, t, o, &expired);

    if (!bank->queues[o->bt]->organ_available) {
        events->organs_loss.reneging_time[o->bt] = INFINITY;
    } else {
        // Generate next renege time for organ
        events->organs_loss.reneging_time[bloodType] = getOrganRenege(bloodType, t->current);
        t->last[organ_renege] = t->current;
    }
}

// --------------------------------------------------- PATIENT LOSS ---------------------------------------------------

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

patient * removePatient(int idx, patient_queue_priority **pQueue, patient_queue_blood_type *pQueueBT,
                        patient_waiting_list *pList, double end_time) {

    patient *prev = NULL;
    patient *current = (*pQueue)->queue;
    patient *next = current->next;

    REMOVE_MID_NODE(idx, current, prev, next)
    current->next = NULL;
    decrementPatients((*pQueue), pQueueBT, pList);

    //pList->waiting_times[current->bt][current->priority] += (end_time - current->start_time);

    return current;
}

void addPatientToLost(event_list *events, sim_time *t, patient *p, patients_lost **pQueue, LOSS_REASON reason) {
    //patient *curr;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    patient_waiting_list *wl = &events->patient_arrival;

    /* roll to last node */
    //GET_LAST_NODE((*pQueue)->queue, curr)

    /* append patient node */
    //curr->next = p;

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
        events->patients_loss.reneging_time[p->bt][p->priority] = INFINITY;
        events->patients_loss.death_time[p->bt][p->priority] = INFINITY;
    } else {
        // generate next loss time (death and renege)
        events->patients_loss.reneging_time[p->bt][p->priority] = getPatientRenege(p->bt, p->priority, t->current);
        events->patients_loss.death_time[p->bt][p->priority] = getPatientDeath(p->priority, p->bt, t->current);
    }

    if (wl->blood_type_queues[p->bt]->number == 0) {
#ifdef AUDIT
        printf("patient queue with blood type %d is now empty -"
               "deactivating living donor arrival event\n", p->bt);
#endif
        events->living_donor_completion[p->bt] = INFINITY;
    }
}

/**
 * TODO
 * */
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr) {

    t->last[patient_loss] = t->current;

    patient_waiting_list *wl = &events->patient_arrival;
    patient_queue_blood_type **pbtQueue = &wl->blood_type_queues[bt];
    patient_queue_priority **ppQueue = &(*pbtQueue)->priority_queue[pr];
    patients_lost *lost = &events->patients_loss;
    patient *p;
    int index, max_number = (int) (*ppQueue)->number;

    // TODO choose a random job or select a specific patient with id or remove oldest one
    index = (reason == death) ? 0 /* FIXME now I remove the oldest one */ : ((int) rand() % (max_number-0+1)) + 0; // FIXME now I choose a random job

    p = removePatient(index, ppQueue, *pbtQueue, wl, t->current);
    if (p != NULL) {
        addPatientToLost(events, t, p, &lost, reason);
#ifdef AUDIT
        printf("A patient has left the queue with priority %d with reason %d\n", (*ppQueue)->priority, reason);
#endif
    }
}

// ------------------------------------------------- MATCHING --------------------------------------------------------

// Internal usage
void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient) {
    in_transplant *curr;

    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;
    transplant_center *tc = &events->transplant_arrival;

    /* Init new match, increment transplant number and add match to transplant queue*/
    matched *m = newMatched(*patient, *organ);
    tc->total_number++;
    in_transplant *in_tr = newTransplant(m, tc->total_number);

    /* generate and change transplant completion time */
    in_tr->completionTime = getTransplantCompletion(t->current);

    /* add new in transplant */
    GET_LAST_NODE(tc->transplanted_patients, curr)
    curr->next = in_tr;

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[patient->bt]->priority_queue[patient->priority]->number == 0) {
        events->patients_loss.reneging_time[patient->bt][patient->priority] = INFINITY;
        events->patients_loss.death_time[patient->bt][patient->priority] = INFINITY;
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty - deactivating death and renege event\n",
               patient->bt, patient->priority);
#endif
    } else {
        events->patients_loss.reneging_time[patient->bt][patient->priority] = getPatientRenege(patient->bt, patient->priority, t->current);
        events->patients_loss.death_time[patient->bt][patient->priority] = getPatientDeath(patient->priority, patient->bt, t->current);
    }

    /* Check if organ queue is empty to eventually deactivate organ renege events */
    if (!bank->queues[organ->bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", organ->bt);
#endif
        events->organs_loss.reneging_time[organ->bt] = INFINITY;
    } else {
        events->organs_loss.reneging_time[organ->bt] = getOrganRenege(organ->bt, t->current);
    }
}

patient *getOldestPatient(const BLOOD_TYPE *pBt, const int size, patient_waiting_list *wl) {
    PRIORITY priority;
    BLOOD_TYPE bloodType;
    double oldest_st, curr_val;
    bool first = true;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            BLOOD_TYPE b = pBt[i];
            if ((wl->blood_type_queues[b]->priority_queue[j]->number != 0)) {

                if (first) {
                    oldest_st = wl->blood_type_queues[b]->priority_queue[j]->queue->next->start_time;
                    priority = j;
                    bloodType = b;
                    first = false;
                } else {
                    curr_val = wl->blood_type_queues[b]->priority_queue[j]->queue->next->start_time;
                    if (curr_val < oldest_st) {
                        oldest_st = curr_val;
                        priority = j;
                        bloodType = b;
                    }
                }
            } else {
                break;
            }
        }
    }

    if (first) return NULL;

    patient *p = removePatient(0,
                               &wl->blood_type_queues[bloodType]->priority_queue[priority],
                               wl->blood_type_queues[bloodType],
                               wl,
                               0);
    return p;
}

bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, organ *organ, bool living) {

    patient *p;
    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;

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
    const BLOOD_TYPE *comp = get_compatibles[bt];
    const int size = get_num_compatibles[bt];

    /*// Serve the rarest blood types first
    int i = 0;
    while (i < NUM_PRIORITIES && !found) {
        for (int j = 0; j < size; ++j) {
            BLOOD_TYPE b = comp[j];
            if ((wl->blood_type_queues[b]->priority_queue[i]->number != 0)) {
                avbPatientBt = b;
                avbPatientPr = i;
                found = true;
                break;
            }
        }
        ++i;
    }*/
    p = getOldestPatient(comp, size, wl);
    if (p != NULL) {
        avbPatientBt = p->bt;
        avbPatientPr = p->priority;
        found = true;
    }

    if (!found) return found;
#endif

    if (!living) {
        /* Add newly arrived organ into queue, recover the oldest organ and handle matching */
        addOrganToQueue(events, t, &bank->queues[bt], organ);
        organ = removeOrgan(0, &bank->queues[bt], bank);
    }

#ifdef ABO_ID
    /* get first patient */
    p = removePatient(0, &wl->blood_type_queues[avbPatientBt]->priority_queue[avbPatientPr],
                               wl->blood_type_queues[avbPatientBt], wl, t->current);
#endif
    addMatchedToTransplant(events, t, organ, p);

    return found;
}

/* Internal usage:
 *      Upon the arrival of a new patient, this function search for a compatible organ in queue.
 *      If an organ is found AND the patient is entitled due priority in having it,
 *          they go to transplant without addition in the waiting list and the organ is removed.
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *patient) {

    PRIORITY pr = patient->priority, priority_placement = pr;
    BLOOD_TYPE avbOrganBt = bt;
    organ_bank *bank = &events->organ_arrival;
    organ_queue **organQueues = bank->queues;
    patient_queue_blood_type **btQueues = events->patient_arrival.blood_type_queues;
    bool found = false;

    // check if the patient can be transplanted due to their priority and other patients in queue
    if (pr == normal || pr == low) {
        if (btQueues[bt]->priority_queue[critical]->number > 0) {
            priority_placement = critical; // serve patient in queue critical
        } else if (pr == low && btQueues[bt]->priority_queue[normal]->number > 0) {
            priority_placement = normal; // serve patient in queue normal
        }
    }

#ifdef ABO_ID
    if (!organQueues[bt]->organ_available) return found;
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

    if (priority_placement != pr) {
        // serve new selected patient and add original patient in queue
        addPatientToQueue(events, t, &events->patient_arrival.blood_type_queues[bt]->priority_queue[pr],
                          events->patient_arrival.blood_type_queues[bt], patient);
        patient = removePatient(0, &events->patient_arrival.blood_type_queues[bt]->priority_queue[priority_placement],
                                events->patient_arrival.blood_type_queues[bt], &events->patient_arrival,
                                t->current);
    }

    // get oldest organ
    organ *o = removeOrgan(0, &bank->queues[avbOrganBt], bank);
    addMatchedToTransplant(events, t, o, patient);

    return found;
}

// ------------------------------------------------- TRANSPLANT ----------------------------------------------

in_transplant *getNextTransplant(double minCompletion, in_transplant *transplanted) {
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        if (transplanted->completionTime == minCompletion) return transplanted;
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

/**
 * TODO
 * */
void handleTransplantCompletion(event_list *events, sim_time *t) {
    in_transplant *transplanted = events->transplant_arrival.transplanted_patients;
    double min = getMinTransplant(transplanted);
    in_transplant *n = getNextTransplant(min, transplanted);

    t->last[transplant] = t->current;

    int idx = (int)n->serverOffset-1;
    in_transplant *prev = NULL;
    in_transplant *curr = transplanted; //head
    in_transplant *next = curr->next; //first inactive

    /* remove completed transplant */
    REMOVE_MID_NODE(idx, curr, prev, next)
    curr->next = NULL;
    //free(curr);
    events->transplant_arrival.total_number--;

    /* update offsets */
    updateTransplantOffsets(transplanted);

    /* reject transplant with a probability below level and send patient back in queue with high priority */
    patient *p = &n->matched->patient;
    double prob = getRejectionProb();
    if (prob < REJECT_P) {
        events->transplant_arrival.rejected_transplants[p->bt][p->priority]++;

        p->priority = critical;
        addToWaitingList(events, t, p);
#ifdef AUDIT
        printf("Patient with blood type %d was rejected and and sent back to waiting list with priority %d\n", p->bt, p->priority);
#endif
    } else {
        events->transplant_arrival.completed_transplants[p->bt][p->priority]++;
    }
}