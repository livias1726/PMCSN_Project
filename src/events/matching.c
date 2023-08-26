#include "../headers/events.h"
#include "../headers/init.h"
#include "../headers/utils.h"

void addMatchedToTransplant(event_list *events, sim_time *t, organ *organ, patient *patient) {
    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;
    transplant_center *tc = &events->transplant_arrival;
    BLOOD_TYPE p_bt = patient->bt, o_bt = organ->bt;
    PRIORITY pr = patient->priority;

    /* Init new match, increment transplant number and add match to transplant queue*/
    matched *m = newMatched(*patient, *organ);
    wl->num_completions[p_bt][pr]++;
    bank->num_completions[o_bt][organ->dt]++;
    tc->total_number++;
    tc->num_arrivals[p_bt][pr]++;
    in_transplant *in_tr = newTransplant(m);

    /* generate and change transplant completion time */
    in_tr->completion_time = getTransplantCompletion(t->current);
    if (in_tr->completion_time < tc->min_transplant) tc->min_transplant = in_tr->completion_time;

    /* add new in transplant */
    in_transplant *last = tc->last_node;
    APPEND_NEW_NODE(tc->transplanted_patients, tc->last_node, last, in_tr)

    /* Check if patient queue is empty to eventually deactivate death and renege events */
    if (wl->blood_type_queues[p_bt]->priority_queue[pr]->number == 0) {
        events->patients_loss.reneging_time[p_bt][pr] = INFINITY;
        events->patients_loss.death_time[p_bt][pr] = INFINITY;
#ifdef AUDIT
        printf("patient queue with blood type %d and priority %d is now empty - deactivating death and renege event\n",
               patient->bt, patient->priority);
#endif
    } else {
        events->patients_loss.reneging_time[p_bt][pr] = getPatientRenege(p_bt, pr, active, t->current);
        events->patients_loss.death_time[p_bt][pr] = getPatientDeath(p_bt, pr, active, t->current);
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
 */
patient *getPatientFromPatient(patient* arrived, patient_waiting_list *wl) {
    PRIORITY pr = arrived->priority;
    BLOOD_TYPE patient_bt = arrived->bt;
    int j;
    patient_queue_priority *pp_queue;
    patient_queue_blood_type *pbt_queue;
    patient *curr;
    bool found = false;

    for (j = 0; j <= pr; ++j) {          // for each priority higher or equal than the new patient's
        // if a patient is available
        pp_queue = wl->blood_type_queues[patient_bt]->priority_queue[j];
        if (pp_queue->number == 0) continue;
        found = true;   // at this point a patient has been found0
        break;
    }

    if (found) {
        pbt_queue = wl->blood_type_queues[patient_bt];
        pp_queue = pbt_queue->priority_queue[j];
        curr = removePatient(0, pp_queue,pbt_queue, wl);
    } else {
        curr = arrived;
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
    int i, j;
    patient_queue_priority *pp_queue;
    patient_queue_blood_type *pbt_queue;
    patient *curr;
    bool found = false;

    BLOOD_TYPE *comp = get_compatibles[organ_bt];     // list of compatible blood types to search for an organ
    const int size = get_num_compatibles[organ_bt];

    for (j = 0; j < NUM_PRIORITIES; ++j){   // for each priority in order from max to min
        for (i = 0; i < size; ++i) {       // for each compatible blood type
            b = comp[i];
            pp_queue = wl->blood_type_queues[b]->priority_queue[j];
            // Check if there are available patients in this specific queue
            if (!pp_queue->number) continue; // test next blood type

            // Check if the retrieved patient is the oldest one yet in the given priority
            curr_val = ((pp_queue->queue)->next)->start_time;
            if (curr_val < oldest_st) {
                oldest_st = curr_val;
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
        curr = removePatient(0, pp_queue,pbt_queue, wl);
    } else {
        curr = NULL;
    }

    return curr;
}

void checkOtherMatching(event_list *events, sim_time *t){
    double organ_num;
    patient *p;
    organ *o;
    patient_waiting_list *wl = &events->patient_arrival;
    organ_bank *bank = &events->organ_arrival;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organ_num = events->organ_arrival.queues[i]->number;
        while (organ_num) {
            p = getPatientFromOrgan(i, wl);
            if (p != NULL) {
                o = removeOrgan(bank->queues[i], bank);
                addMatchedToTransplant(events, t, o, p);
            }
            organ_num--;
        }
    }
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
    p = removePatient(0, pp_queue, pbt_queue, wl);
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

    checkOtherMatching(events, t);

    return true;
}

/* *
 * Upon the arrival of a new patient, this function search for a compatible organ in queue.
 * If an organ is found AND the patient is entitled due priority in having it,
 * they go to transplant without addition in the waiting list and the organ is removed.
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
#ifdef IMPROVEMENT
        if (o_queues[i]->organ_available) {
#else
        if (o_queues[i]->organ_available && (COMPATIBLE(i, patient_bt))) {
#endif
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

    checkOtherMatching(events, t);

    return true;
}