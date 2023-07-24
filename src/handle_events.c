#include "headers/utils.h"

/***
 * This module contains the implementation of every EVENT HANDLER for the system ---------------------------------------
 */

// Internal MATCHING
bool handleMatchingFromOrgan(event_list *, sim_time *, BLOOD_TYPE, organ *, bool living);
bool handleMatchingFromPatient(event_list*, sim_time*, BLOOD_TYPE, patient*);


// ------------------------------------------ ORGAN ARRIVAL -----------------------------------------------------------

//patient *getOldestPatient(const BLOOD_TYPE *pBt, const int size, patient_waiting_list *wl);

// Internal usage: adds an organ to the organ queue with a specific blood type
void addOrganToQueue(event_list *events, sim_time *t, organ_queue *o_queue, organ *o) {

    /* Increment organs */
    o_queue->number++;
    if (!o_queue->organ_available) o_queue->organ_available = true;
    (&events->organ_arrival)->total_number++;

    /* Add new organ */
    organ *curr;
    GET_LAST_NODE(o_queue->queue, curr)
    curr->next = o;

    /* Check if there is just one organ to eventually activate next organ renege */
    BLOOD_TYPE bt = o->bt;
    if (o_queue->number == 1) {
#ifdef AUDIT
        printf("Only one organ in queue - activating renege of organs with blood type %d\n", o->bt);
#endif
        events->organs_loss.reneging_time[bt] = getOrganRenege(bt, t->current);
    }
}

/***
 * Handler of a new organ's arrival:
 *      - Checks the availability of a compatible patient in waiting list and eventually triggers a matching event
 *      - If no compatible patient is available, adds the organ in the specific blood type queue of the organ bank
 *
 * @param events
 * @param t
 * @param bt
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, bool living_flag) {

    organ *o;
    organ_bank *bank = &events->organ_arrival;

    /* Generate next arrival time of an organ */
    if (living_flag) {
        /* living donor organ */
        events->living_donor_completion[bt] = getLivingDonorOrganArrival(bt, t->current);
        o = newOrgan(bt, living);
    } else {
        /* deceased donor organ */
        bank->inter_arrival_time[bt] = getDecDonorOrganArrival(bt, t->current);
        o = newOrgan(bt, deceased);
    }
    t->last[organ_arrival] = t->current;

    /* increment arrivals */
    bank->num_arrivals[bt]++;

    /* Check for matching */
    bool match = handleMatchingFromOrgan(events, t, bt, o, living_flag);

    if (!match && !living_flag){
        /* Add new organ to the bank */
        addOrganToQueue(events, t, bank->queues[bt], o);
#ifdef AUDIT
        printf("Arrived organ with blood type %s\n", bt_to_str[bt]);
    } else {
        printf("Arrived and dispatched organ with blood type %s\n", bt_to_str[bt]);
#endif
    }
}

// ------------------------------------------------- PATIENT ARRIVAL ------------------------------------------------

// Internal usage: adds the patient to a specific queue with a specific priority
void addPatientToQueue(event_list *events, sim_time *t, patient_queue_priority *pp_queue,
                       patient_queue_blood_type *bt_queue, patient *p) {

    patient *curr;
    patient_waiting_list *wl = &events->patient_arrival;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    double curr_time = t->current;

    /* Increment patients numbers */
    pp_queue->number++;
    bt_queue->number++;
    wl->total_number++;
    if (!bt_queue->patient_available) bt_queue->patient_available = true;

    /* increment arrivals */
    wl->num_arrivals[bt][pr]++;

    /* set patient arrival time*/
    p->start_time = curr_time;

    /* add new patient */
    GET_LAST_NODE(pp_queue->queue, curr)
    curr->next = p;

    /* Check if there is just one patient in queue to eventually activate patient death and renege */
    if (pp_queue->number == 1) {
        events->patients_loss.reneging_time[bt][pr] = getPatientRenege(bt, pr, curr_time);
        events->patients_loss.death_time[bt][pr] = getPatientDeath(bt, pr, curr_time);
#ifdef AUDIT
        printf("Only one patient in queue - activating patient death and renege with blood type %d and priority %d\n",
               p->bt, p->priority);
#endif
    }

    if (bt_queue->number == 1) {
        events->living_donor_completion[bt] = getLivingDonorOrganArrival(bt, curr_time);
#ifdef AUDIT
        printf("Only one patient in blood type queue - activating living donor arrival with blood type %d\n", p->bt);
#endif
    }
}

// Internal usage: adds the patient to a specific queue with a specific priority
void addToWaitingList(event_list *events, sim_time* t, patient *p) {
    patient_waiting_list *wl = &events->patient_arrival;
    BLOOD_TYPE bt = p->bt;
    PRIORITY priority = p->priority;

    bool match = handleMatchingFromPatient(events, t, bt, p);

    if (!match) {
        patient_queue_blood_type *pbt_queue = wl->blood_type_queues[bt];
        patient_queue_priority *pp_queue = pbt_queue->priority_queue[priority];

        addPatientToQueue(events, t, pp_queue, pbt_queue, p);
#ifdef AUDIT
        printf("Arrived patient with blood type %s and priority %s\n", bt_to_str[bt], pr_to_str[priority]);
    } else {
        printf("Arrived and served patient with blood type %s and priority %s\n", bt_to_str[bt], pr_to_str[priority]);
#endif
    }
}

// Internal usage: adds inactive patient to the activation_center center
void addToActivationCenter(event_list *events, sim_time *t, patient *p) {
    activation_center *ac = &events->activation_arrival;
    /* increment number inactive */
    ac->total_number++;
    /* add patient to activation_center queue */
    in_activation *inactive = newInactive(p, ac->total_number);
    /* generate and change activation_center time */
    inactive->completionTime = getActivationCompletion(t->current);

    /* add to activation_center servers */
    in_activation *curr;
    GET_LAST_NODE(ac->inactive_patients, curr)
    curr->next = inactive;
}

/***
 * Handler of a new patient's arrival:
 *      - Manages the event list according to the event of a patient's arrival
 *      - If the patient has low pr, they are added as 'inactive' to the activation_center center
 *      - Else, checks the availability of a compatible organ in the bank and eventually triggers a matching event
 *      - If no compatible organ is available, adds the patient in the waiting list
 *
 * @param events
 * @param t
 * @param bt
 * @param pr
 */
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr) {

    // Generate next arrival time of a patient
    events->patient_arrival.inter_arrival_time[bt][pr] = getPatientArrival(bt, pr, t->current);
    t->last[patient_arrival] = t->current;

    /* New patient */
    patient* p = newPatient(bt, pr);

    /* Check if the patient has pr low - it means that has to get activated first! */
    if (pr == low) {
        addToActivationCenter(events, t, p);
    } else {
        addToWaitingList(events, t, p);
    }
}

// -------------------------------------------------- ACTIVATION----------------------------------------------------

in_activation *getNextActivation(double min_completion, in_activation* inactive) {
    while (inactive->next != NULL) {
        inactive = inactive->next;
        if (inactive->completionTime == min_completion) return inactive;
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

    events->activation_arrival.total_number--;
    events->activation_arrival.activated_number[curr->patient->bt]++;

    curr->next = NULL;

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

void decrementOrgans(organ_queue *queue, organ_bank *bank) {
    if (queue->number > 0) queue->number--;

    if (bank->total_number > 0) bank->total_number--;

    if (queue->number == 0) {
        queue->organ_available = false;
#ifdef AUDIT
        printf("No organs left in queue with blood type %s\n", bt_to_str[queue->bt]);
#endif
    }
}

organ * removeOrgan(int idx, organ_queue *queue, organ_bank *bank) {

    organ *prev = NULL;
    organ *current = queue->queue; //head
    organ *next = current->next; //first organ

    REMOVE_MID_NODE(idx, current, prev, next)
    current->next = NULL;

    decrementOrgans(queue, bank);

    return current;
}

void addOrganToLost(event_list *events, sim_time *t, organ *o, organs_expired *exp_queue) {
    //organ *curr;
    BLOOD_TYPE bt = o->bt;
    organ_bank *bank = &events->organ_arrival;

    /* roll to last node */
    //GET_LAST_NODE((*exp_queue)->queue, curr)

    /* append organ node */
    //curr->next = o;

    /* increment loss */
    exp_queue->number[bt]++;

    /* Check if the organ queue is empty to eventually deactivate loss event */
    if (!bank->queues[bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty - deactivating renege event\n", o->bt);
#endif
        events->organs_loss.reneging_time[bt] = INFINITY;
    } else {
        // generate next organ loss time
        events->organs_loss.reneging_time[bt] = getOrganRenege(bt, t->current);
    }

    free(o); // TODO: to remove if a queue of lost organs is used
}

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 * @param bt
 * @param pBank
 */
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bt) {

    organ_bank *bank = &events->organ_arrival;
    organs_expired *expired = &events->organs_loss;
    
    /* Remove the oldest organ in the queue */
    organ *o = removeOrgan(0, bank->queues[bt], bank);
    addOrganToLost(events, t, o, expired);

    if (!bank->queues[bt]->organ_available) {
        events->organs_loss.reneging_time[bt] = INFINITY;
    } else {
        // Generate next renege time for organ
        events->organs_loss.reneging_time[bt] = getOrganRenege(bt, t->current);
        t->last[organ_renege] = t->current;
    }
}

// --------------------------------------------------- PATIENT LOSS ---------------------------------------------------

void decrementPatients(patient_queue_priority *pp_queue, patient_queue_blood_type *bt_queue, patient_waiting_list *wl) {

    if (pp_queue->number > 0) pp_queue->number--;

    if (bt_queue->number > 0) bt_queue->number--;

    if (wl->total_number > 0) wl->total_number--;
}

patient * removePatient(int idx, patient_queue_priority *pp_queue, patient_queue_blood_type *bt_queue,
                        patient_waiting_list *wl) {

    patient *prev = NULL;
    patient *current = pp_queue->queue;
    patient *next = current->next;

    REMOVE_MID_NODE(idx, current, prev, next)
    current->next = NULL;

    decrementPatients(pp_queue, bt_queue, wl);

    return current;
}

void addPatientToLost(event_list *events, sim_time *t, patient *p, patients_lost *lost_queue, LOSS_REASON reason) {
    //patient *curr;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    patient_queue_blood_type *pbt_queue = events->patient_arrival.blood_type_queues[bt];

    /* roll to last node */
    //GET_LAST_NODE((*lost_queue)->queue, curr)

    /* append patient node */
    //curr->next = p;

    /* increment loss */
    if (reason == death) {
        lost_queue->number_dead[bt][pr]++;
    } else {
        lost_queue->number_renege[bt][pr]++;
    }

    /* Check if there are other patients in queue to eventually deactivate reneging and death events */
    if (pbt_queue->priority_queue[pr]->number == 0) {
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty -"
               "deactivating death and renege event\n", p->bt, p->priority);
#endif
        lost_queue->reneging_time[bt][pr] = INFINITY;
        lost_queue->death_time[bt][pr] = INFINITY;
    } else {
        // generate next loss time (death and renege)
        lost_queue->reneging_time[bt][pr] = getPatientRenege(bt, pr, t->current);
        lost_queue->death_time[bt][pr] = getPatientDeath(bt, pr, t->current);
    }

    if (pbt_queue->number == 0) {
#ifdef AUDIT
        printf("patient queue with blood type %d is now empty -"
               "deactivating living donor arrival event\n", p->bt);
#endif
        events->living_donor_completion[bt] = INFINITY;
    }
}

/**
 * Manages the event of a patient renege due to death or other reasons
 * */
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr) {

    t->last[patient_loss] = t->current;

    patient_waiting_list *wl = &events->patient_arrival;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[bt];
    patient_queue_priority *pp_queue = pbt_queue->priority_queue[pr];
    patients_lost *lost = &events->patients_loss;
    patient *p;

    int index, max_number = (int) pp_queue->number;

    // TODO choose a random job or select a specific patient with id or remove oldest one
    if (reason == death) {
        index = 0; /* FIXME now I remove the oldest one */
    } else {
        srand(time(NULL));
        index = ((int) rand() % (max_number+1)); // FIXME now I choose a random job
    }

    p = removePatient(index, pp_queue, pbt_queue, wl);
    if (p != NULL) {
        addPatientToLost(events, t, p, lost, reason);
#ifdef AUDIT
        printf("A patient has left the queue with priority %d with reason %d\n", (*pp_queue)->priority, reason);
#endif
    }
}

// ------------------------------------------------- MATCHING --------------------------------------------------------

// Internal usage
void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient) {
    in_transplant *curr, *in_tr;
    matched  *m;

    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;
    transplant_center *tc = &events->transplant_arrival;

    BLOOD_TYPE p_bt = patient->bt;
    BLOOD_TYPE o_bt = organ->bt;
    PRIORITY pr = patient->priority;

    /* Init new match, increment transplant number and add match to transplant queue*/
    m = newMatched(*patient, *organ);
    tc->total_number++;
    wl->num_completions[p_bt][pr]++;
    in_tr = newTransplant(m, tc->total_number);

    /* generate and change transplant completion time */
    in_tr->completionTime = getTransplantCompletion(t->current);

    /* add new in transplant */
    GET_LAST_NODE(tc->transplanted_patients, curr)
    curr->next = in_tr;

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[p_bt]->priority_queue[pr]->number == 0) {
        events->patients_loss.reneging_time[p_bt][pr] = INFINITY;
        events->patients_loss.death_time[p_bt][pr] = INFINITY;
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty - deactivating death and renege event\n",
               patient->p_bt, patient->priority);
#endif
    } else {
        events->patients_loss.reneging_time[p_bt][pr] = getPatientRenege(p_bt, pr, t->current);
        events->patients_loss.death_time[p_bt][pr] = getPatientDeath(p_bt, pr, t->current);
    }

    /* Check if organ queue is empty to eventually deactivate organ renege events */
    if (!bank->queues[o_bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", organ->p_bt);
#endif
        events->organs_loss.reneging_time[o_bt] = INFINITY;
    } else {
        events->organs_loss.reneging_time[o_bt] = getOrganRenege(o_bt, t->current);
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
                    break;
                } else {
                    curr_val = wl->blood_type_queues[b]->priority_queue[j]->queue->next->start_time;
                    if (curr_val < oldest_st) {
                        oldest_st = curr_val;
                        priority = j;
                        bloodType = b;
                    }
                    break;
                }
            }
        }
    }

    if (first) return NULL;

    patient *p = removePatient(0,wl->blood_type_queues[bloodType]->priority_queue[priority],
                               wl->blood_type_queues[bloodType], wl);
    return p;
}

bool handleMatchingFromOrgan(event_list *events, sim_time *t, BLOOD_TYPE bt, organ *organ, bool living) {

    patient *p;
    patient_waiting_list *wl = &events->patient_arrival;

    organ_bank *bank = &events->organ_arrival;

#ifdef ABO_ID
    PRIORITY patient_pr = none;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[bt];

    if (pbt_queue->number == 0) return false; // no patient in list for a ABO_identical transplant

    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        if (pbt_queue->priority_queue[i]->number != 0) {
            patient_pr = i;
            break;
        }
    }
#else
    const BLOOD_TYPE *comp = get_compatibles[bt];
    const int size = get_num_compatibles[bt];

    p = getOldestPatient(comp, size, wl);
    if (p == NULL) {
        return false;
    }
#endif

    if (!living) {
        if (bank->queues[bt]->number > 0) {
            /* Add newly arrived organ into queue, recover the oldest organ and handle matching */
            addOrganToQueue(events, t, bank->queues[bt], organ);
            organ = removeOrgan(0, bank->queues[bt], bank);
        }
    }

#ifdef ABO_ID
    /* get first patient */
    p = removePatient(0, pbt_queue->priority_queue[patient_pr],pbt_queue, wl);
#endif

    addMatchedToTransplant(events, t, organ, p);

    return true;
}

/* Internal usage:
 *      Upon the arrival of a new patient, this function search for a compatible organ in queue.
 *      If an organ is found AND the patient is entitled due priority in having it,
 *          they go to transplant without addition in the waiting list and the organ is removed.
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *patient) {

    PRIORITY pr = patient->priority;    // priority of the new patient
    PRIORITY priority_placement = pr;   // priority to serve if an organ is available
    BLOOD_TYPE organ_bt = bt;           // blood type of the available organ to use (same as the patient if ABO_ID)

    organ_bank *bank = &events->organ_arrival;
    patient_waiting_list *wl = &events->patient_arrival;
    organ_queue **o_queues = bank->queues;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[bt];
    patient_queue_priority **pp_queues = pbt_queue->priority_queue;

    bool found = false;

    // check if the patient can be transplanted due to their priority and other patients in queue
    if (pr == normal || pr == low) {
        if (pp_queues[critical]->number > 0) {
            priority_placement = critical; // serve patient in queue critical
        } else if (pr == low && pp_queues[normal]->number > 0) {
            priority_placement = normal; // serve patient in queue normal
        }
    }

#ifdef ABO_ID
    found = o_queues[bt]->organ_available;
#else
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if ((o_queues[i]->organ_available) && (COMPATIBLE(i, bt))) {
            organ_bt = i;
            found = true;
            break;
        }
    }
#endif
    if (!found) return found;

    if (priority_placement != pr) {
        // serve new selected patient and add original patient in queue
        addPatientToQueue(events, t, pp_queues[pr], pbt_queue,patient);
        patient = removePatient(0, pp_queues[priority_placement],pbt_queue,wl);
    }

    // get oldest organ
    organ *o = removeOrgan(0, o_queues[organ_bt], bank);
    addMatchedToTransplant(events, t, o, patient);

    return found;
}

// ------------------------------------------------- TRANSPLANT ----------------------------------------------

in_transplant *getNextTransplant(double min_completion, in_transplant *transplanted) {
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        if (transplanted->completionTime == min_completion) {
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

/**
 * Completes a transplant that was previously enqueued during handling of a match
 * */
void handleTransplantCompletion(event_list *events, sim_time *t) {
    in_transplant *transplanted = events->transplant_arrival.transplanted_patients;
    double min = getMinTransplant(transplanted);
    in_transplant *n = getNextTransplant(min, transplanted);

    t->last[transplant] = t->current;

    /* remove completed transplant */
    int idx = (int)n->serverOffset-1;
    in_transplant *prev = NULL;
    in_transplant *curr = transplanted; //head
    in_transplant *next = curr->next; //first inactive

    REMOVE_MID_NODE(idx, curr, prev, next)
    curr->next = NULL;

    events->transplant_arrival.total_number--;

    /* update offsets */
    updateTransplantOffsets(transplanted);

    /* reject transplant with a probability below level and send patient back in queue with high priority */
    patient *p = &n->matched->patient;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    double prob = getRejectionProb();

    if (prob < REJECT_P) {
        events->transplant_arrival.rejected_transplants[bt][pr]++;
        p->priority = critical;
        addToWaitingList(events, t, p);
#ifdef AUDIT
        printf("Patient with blood type %d was rejected and and sent back to waiting list with priority %d\n", p->bt, p->priority);
#endif
    } else {
        events->transplant_arrival.completed_transplants[bt][pr]++;
    }
}