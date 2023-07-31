#include "headers/utils.h"

/***
 * This module contains the implementation of every EVENT HANDLER for the system ---------------------------------------
 */

// Internal
bool handleMatchingFromOrgan(event_list *, sim_time *, organ *);
bool handleMatchingFromPatient(event_list *, sim_time *, patient *);
patient *getPatientFromOrgan(BLOOD_TYPE, patient_waiting_list*);


// ------------------------------------------ ORGAN ARRIVAL -----------------------------------------------------------

// Internal usage: adds an organ to the organ queue with a specific blood type
void addOrganToQueue(event_list *events, sim_time *t, organ_queue *o_queue, organ *o) {

    /* Increment organs */
    o_queue->number++;
    if (!o_queue->organ_available) o_queue->organ_available = true;
    (&events->organ_arrival)->total_number++;

    /* Add new organ */
    organ *last = o_queue->last_organ;
    if (last == NULL) {
        o_queue->queue->next = o;
    } else {
        last->next = o;
    }
    o_queue->last_organ = o;

    /* Check if there is just one organ to eventually activate next organ renege */
    BLOOD_TYPE bt = o->bt;
    if (o_queue->number == 1) {
#ifdef AUDIT
        printf("Only one organ in queue - activating renege of organs with blood type %d\n", o->bt);
#endif
        events->organs_loss.reneging_time[bt] = getOrganRenege(t->current);
    }
}

/***
 * Handler of a new organ's arrival:
 *      - Checks the availability of a compatible patient in waiting list and eventually triggers a matching event
 *      - If no compatible patient is available, adds the organ in the specific blood type queue of the organ bank
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, DONOR_TYPE dt) {

    t->last[organ_arrival] = t->current;

    organ *o;
    organ_bank *bank = &events->organ_arrival;

    /* Generate next arrival time of an organ */
    if (dt == living) {
        /* living donor organ */
        events->living_donor_completion[bt] = getOrganArrival(bt, living, t->current);
    } else {
        /* deceased donor organ */
        bank->inter_arrival_time[bt] = getOrganArrival(bt, deceased, t->current);
    }

    o = newOrgan(bt, dt);

    /* increment arrivals */
    bank->num_arrivals[bt]++;

    /* Check for matching */
    bool match = handleMatchingFromOrgan(events, t, o);

    if (!match && dt == deceased){
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

    patient_waiting_list *wl = &events->patient_arrival;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    double curr_time = t->current;

    /* Increment patients numbers */
    pp_queue->number++;
    bt_queue->number++;
    wl->total_number++;

    /* increment arrivals */
    wl->num_arrivals[bt][pr]++;

    /* set patient arrival time*/
    p->start_time = curr_time;

    /* add new patient */
    patient *last = pp_queue->last;
    if (last == NULL) {
        pp_queue->queue->next = p;
    } else {
        last->next = p;
    }
    pp_queue->last = p;

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
        events->living_donor_completion[bt] = getOrganArrival(bt, living, curr_time);
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

    bool match = handleMatchingFromPatient(events, t, p);

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

/***
 * Handler of a new patient's arrival:
 *      - Manages the event list according to the event of a patient's arrival
 *      - If the patient has inactive pr, they are added as 'inactive' to the activation_center center
 *      - Else, checks the availability of a compatible organ in the bank and eventually triggers a matching event
 *      - If no compatible organ is available, adds the patient in the waiting list
 *
 * @param events
 * @param t
 * @param bt
 * @param pr
 */
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr) {

    t->last[patient_arrival] = t->current;

    // Generate next arrival time of a patient
    events->patient_arrival.inter_arrival_time[bt][pr] = getPatientArrival(bt, pr, t->current);

    /* New patient */
    patient* p = newPatient(bt, pr);

    addToWaitingList(events, t, p);
}

// --------------------------------------------------- ORGAN RENEGE ---------------------------------------------------

// always pop organ from the head of the queue
organ * removeOrgan(organ_queue *queue, organ_bank *bank) {

    organ *head = queue->queue, *first = head->next;
    POP_NODE(head,first)

    if (queue->number > 0) queue->number--;
    if (bank->total_number > 0) bank->total_number--;
    if (queue->number == 0) {
        queue->organ_available = false;
#ifdef AUDIT
        printf("No organs left in queue with blood type %s\n", bt_to_str[queue->bt]);
#endif
    }

    if (first == queue->last_organ) queue->last_organ = NULL;

    return first;
}

void addOrganToLost(event_list *events, sim_time *t, organ *o, organs_expired *exp_queue) {

    BLOOD_TYPE bt = o->bt;
    organ_bank *bank = &events->organ_arrival;

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
        events->organs_loss.reneging_time[bt] = getOrganRenege(t->current);
    }

    free(o);
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
    organ *o = removeOrgan(bank->queues[bt], bank);
    addOrganToLost(events, t, o, expired);

    if (!bank->queues[bt]->organ_available) {
        events->organs_loss.reneging_time[bt] = INFINITY;
    } else {
        // Generate next renege time for organ
        events->organs_loss.reneging_time[bt] = getOrganRenege(t->current);
        t->last[organ_renege] = t->current;
    }
}

// --------------------------------------------------- PATIENT LOSS ---------------------------------------------------

patient * removePatient(int idx, patient_queue_priority *pp_queue, patient_queue_blood_type *bt_queue,
                        patient_waiting_list *wl) {

    patient *prev = NULL, *current = pp_queue->queue, *next = current->next, *removed;

    if (idx == 0) {
        POP_NODE(current, next)
        if (next == pp_queue->last) pp_queue->last = NULL;
        removed = next;
    } else {
        REMOVE_MID_NODE(idx, prev, current, next)
        removed = current;
    }

    if (pp_queue->number > 0) pp_queue->number--;
    if (bt_queue->number > 0) bt_queue->number--;
    if (wl->total_number > 0) wl->total_number--;

    return removed;
}

void addPatientToLost(event_list *events, sim_time *t, patient *p, patients_lost *lost_queue, LOSS_REASON reason) {

    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    patient_queue_blood_type *pbt_queue = events->patient_arrival.blood_type_queues[bt];

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

    patients_lost *lost = &events->patients_loss;
    int index, max_number;
    patient *p;

    patient_waiting_list *wl = &events->patient_arrival;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[bt];
    patient_queue_priority *pp_queue = pbt_queue->priority_queue[pr];

    max_number = (int) pp_queue->number;

    // choose a random job or remove the oldest one
    if (reason == death) {
        index = 0; /* remove the oldest one */
    } else {
        new_srand(time(NULL));
        index = new_rand(max_number); /* choose a random patient */
    }

    p = removePatient(index, pp_queue, pbt_queue, wl);
    if (p != NULL) {
        addPatientToLost(events, t, p, lost, reason);
#ifdef AUDIT
        printf("A patient has left the queue with priority %s with reason %d\n", pr_to_str[priority], reason);
#endif
    }
}

// ------------------------------------------------- MATCHING --------------------------------------------------------

// Internal usage
void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient) {
    in_transplant *in_tr;
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
    in_tr = newTransplant(m);

    /* generate and change transplant completion time */
    in_tr->completion_time = getTransplantCompletion(t->current);
    if (in_tr->completion_time < tc->min_transplant) tc->min_transplant = in_tr->completion_time;

    /* add new in transplant */
    in_transplant *last = tc->last_node;
    if (last == NULL) {
        tc->transplanted_patients->next = in_tr;
    } else {
        last->next = in_tr;
    }
    tc->last_node = in_tr;

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[p_bt]->priority_queue[pr]->number == 0) {
        events->patients_loss.reneging_time[p_bt][pr] = INFINITY;
        events->patients_loss.death_time[p_bt][pr] = INFINITY;
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty - deactivating death and renege event\n",
               patient->bt, patient->priority);
#endif
    } else {
        events->patients_loss.reneging_time[p_bt][pr] = getPatientRenege(p_bt, pr, t->current);
        events->patients_loss.death_time[p_bt][pr] = getPatientDeath(p_bt, pr, t->current);
    }

    /* Check if organ queue is empty to eventually deactivate organ renege events */
    if (!bank->queues[o_bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty -"
               "deactivating renege event\n", organ->bt);
#endif
        events->organs_loss.reneging_time[o_bt] = INFINITY;
    } else {
        events->organs_loss.reneging_time[o_bt] = getOrganRenege(t->current);
    }
}

/*
 * After a new patient arrival, the organ bank gets scanned for a compatible organ and a patient
 * (not necessarily the new one) gets transplanted, if a total compatibility is found
 *
 * fixme: temporary management -> the patient that will get a transplant will have the same blood type of the new one
 */
patient *getPatientFromPatient(patient* arrived, patient_waiting_list *wl) {
    PRIORITY pr = arrived->priority;
    BLOOD_TYPE patient_bt = arrived->bt;
    int tot_in_queue, j, to_transplant, idx;
    patient_queue_priority *pp_queue;
    patient_queue_blood_type *pbt_queue;
    patient *curr;
    bool no_trans = true, found = false;

    for (j = 0; j <= pr; ++j) {          // for each priority higher or equal than the new patient's
        // if a patient is available
        pp_queue = wl->blood_type_queues[patient_bt]->priority_queue[j];
        tot_in_queue = (int) pp_queue->number;
        idx = -1;
        if (tot_in_queue == 0) continue; // test next blood type
        curr = pp_queue->queue; // get the head of the queue

        // Get the oldest in this queue (scan from head to tail) that can go to transplant
        do {
            curr = curr->next;
            idx++;
            no_trans = (getTransplantProb(patient_bt) < TRANSPLANT_PROB[patient_bt]); // check for complete compatibility
            tot_in_queue--;
        } while (tot_in_queue > 0 && no_trans);

        if (!no_trans) { // a patient with complete compatibility was found for this blood type
            found = true;   // at this point a patient has been found
            to_transplant = idx;
            break;
        }
    }

    if (found) {
        pbt_queue = wl->blood_type_queues[patient_bt];
        pp_queue = pbt_queue->priority_queue[j];
        curr = removePatient(to_transplant, pp_queue,pbt_queue, wl);
    } else {
        // if no patient already in queue was suitable for transplant, test the complete compatibility of the new one
        if(getTransplantProb(patient_bt) < TRANSPLANT_PROB[patient_bt]){
            curr = NULL;
        } else {
            curr = arrived;
        }
    }

    return curr;
}

/*
 * Iterate over the waitlist in order of priority and waitlist addition time for each compatible blood type
 * and select a TOTALLY COMPATIBLE patient, checking also a transplant probability, representing a
 * negative cross-match (compatible HLA) [See Alvin E. Roth - Kidney Exchange]
 * */
patient *getPatientFromOrgan(BLOOD_TYPE organ_bt, patient_waiting_list *wl) {
    PRIORITY pr;
    BLOOD_TYPE b, bt;
    double oldest_st = INFINITY, curr_val;
    int tot_in_queue, i, j, to_transplant, idx;
    patient_queue_priority *pp_queue;
    patient_queue_blood_type *pbt_queue;
    patient *curr;
    bool found = false, no_trans = false;

    BLOOD_TYPE *comp = get_compatibles[organ_bt];     // list of compatible blood types to search for an organ
    const int size = get_num_compatibles[organ_bt];

    for (j = 0; j < NUM_PRIORITIES; ++j){   // for each priority in order from max to min
         for (i = 0; i < size; ++i) {       // for each compatible blood type
             b = comp[i];
             pp_queue = wl->blood_type_queues[b]->priority_queue[j];
             tot_in_queue = (int)pp_queue->number;
             idx = -1;

             // Check if there are available patients in this specific queue
             if (tot_in_queue == 0) continue; // test next blood type
             curr = pp_queue->queue; // get the head of the queue

             // Get the oldest in this queue (scan from head to tail) that can go to transplant
             do{
                 curr = curr->next;
                 idx++;
                 no_trans = (getTransplantProb(b) < TRANSPLANT_PROB[b]); // check for complete compatibility
                 tot_in_queue--;
             } while (tot_in_queue > 0 && no_trans);
             if (no_trans) continue; // no patient with complete compatibility was found for this blood type

             // Check if the retrieved patient is the oldest one yet in the given priority
             curr_val = curr->start_time;
             if (curr_val < oldest_st) {
                 oldest_st = curr_val;
                 to_transplant = idx;
                 pr = j;
                 bt = b;
                 found = true;
             }
         }
         if (found) break;  /* at least one totally compatible patient (the oldest) was found
                             * in the given priority for each compatible bt */
    }

    if (found) {
        pbt_queue = wl->blood_type_queues[bt];
        pp_queue = pbt_queue->priority_queue[pr];
        curr = removePatient(to_transplant, pp_queue,pbt_queue, wl);
    } else {
        curr = NULL;
    }

    return curr;
}

bool handleMatchingFromOrgan(event_list *events, sim_time *t, organ *organ) {

    patient *p;
    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;

    BLOOD_TYPE organ_bt = organ->bt;

#ifdef ABO_ID
    PRIORITY pr = none;
    BLOOD_TYPE patient_bt = organ_bt;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[patient_bt];
    if (pbt_queue->number == 0) return false; // no patient in list for a ABO_identical transplant

    for (int i = 0; i < NUM_PRIORITIES; ++i) {
        if (pbt_queue->priority_queue[i]->number != 0) {
            pr = i;
            break;
        }
    }
    if (pr == none) return false;

    pbt_queue = wl->blood_type_queues[patient_bt];
    patient_queue_priority *pp_queue = pbt_queue->priority_queue[pr];

    /* get first patient */
    p = removePatient(0, pp_queue,pbt_queue, wl);
#else
    p = getPatientFromOrgan(organ_bt, wl);
    if (p == NULL) return false;
#endif

    // if the organ is not from a living donor (which has a specific receiver), get the oldest one
    if (organ->dt == deceased) {
        if (bank->queues[organ_bt]->number > 0) {
            /* Add newly arrived organ into queue, recover the oldest organ and handle matching */
            addOrganToQueue(events, t, bank->queues[organ_bt], organ);
            organ = removeOrgan(bank->queues[organ_bt], bank);
        }
    }

    addMatchedToTransplant(events, t, organ, p);

    return true;
}

/* Internal usage:
 *      Upon the arrival of a new p, this function search for a compatible organ in queue.
 *      If an organ is found AND the p is entitled due priority in having it,
 *          they go to transplant without addition in the waiting list and the organ is removed.
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, patient *p) {

    PRIORITY pr = p->priority;
    BLOOD_TYPE organ_bt, patient_bt = p->bt;

    organ_bank *bank = &events->organ_arrival;
    patient_waiting_list *wl = &events->patient_arrival;
    organ_queue **o_queues = bank->queues;
    patient_queue_blood_type *pbt_queue = wl->blood_type_queues[patient_bt];
    patient_queue_priority **pp_queues = pbt_queue->priority_queue;

    patient *to_transplant;

#ifdef ABO_ID
    to_transplant = p;
    organ_bt = patient_bt;
    if (!o_queues[organ_bt]->organ_available) return false;

    // check if the p can be transplanted due to their priority and other patients in queue
    for (int j = 0; j < pr; ++j) {
        if (pp_queues[j]->number > 0) {
            to_transplant = pp_queues[j]->queue->next;
        }
    }
#else
    // search for a compatible organ available
    bool o_available = false;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if (o_queues[i]->organ_available && (COMPATIBLE(i, patient_bt))) {
            organ_bt = i;
            o_available = true; // found a compatible organ available
            break;
        }
    }
    if (!o_available) return false;

    to_transplant = getPatientFromPatient(p, wl);
    if (to_transplant == NULL) return false;
#endif

    if (p != to_transplant) {
        // serve new selected p and add original p in queue
        addPatientToQueue(events, t, pp_queues[pr], pbt_queue, p);
        p = to_transplant;
    }

    // get oldest organ
    organ *o = removeOrgan(o_queues[organ_bt], bank);
    addMatchedToTransplant(events, t, o, p);

    return true;
}

// ------------------------------------------------- TRANSPLANT ----------------------------------------------

in_transplant *getNextTransplant(double min_completion, transplant_center *tc) {
    in_transplant *prev = NULL, *curr = tc->transplanted_patients, *next = curr->next, *to_transplant = NULL;
    double new_min = INFINITY;

    while (next != NULL) {
        prev = curr;
        curr = next;
        next = curr->next;

        if (curr->completion_time == min_completion) {
            prev->next = next;  /* unlink the node */
            curr->next = NULL;
            if (curr == tc->last_node) tc->last_node = prev;
            to_transplant = curr;
        } else {
            if (curr->completion_time < new_min) new_min = curr->completion_time;
        }
    }

    tc->min_transplant = new_min;

    return to_transplant;
}

/**
 * Completes a transplant that was previously enqueued during handling of a match
 * */
void handleTransplantCompletion(event_list *events, sim_time *t) {

    t->last[transplant] = t->current;

    in_transplant *to_transplant = getNextTransplant(t->current, &events->transplant_arrival);

    /* reject transplant with a probability below level and send patient back in queue with high priority */
    patient *p = &to_transplant->matched->patient;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    double prob = getRejectionProb();

    if (prob < REJECT_P) {
        events->transplant_arrival.rejected_transplants[bt][pr]++;
        addToWaitingList(events, t, p);
#ifdef AUDIT
        printf("Patient with blood type %s was rejected and and sent back to waiting list with priority %s\n",
               bt_to_str[p->bt], pr_to_str[p->priority]);
#endif
    } else {
        events->transplant_arrival.completed_transplants[bt][pr]++;
    }

    events->transplant_arrival.total_number--;
    free(to_transplant);
}