#include "../headers/events.h"
#include "../headers/utils.h"

void updateMinActivation(activation_queue *aq, activation_center *ac){
    if (aq->number == 0) {
        aq->min_activation = INFINITY;
    } else {
        in_activation *curr = aq->head, *next = curr->next;
        double new_min = INFINITY;

        while (next != NULL) {
            curr = next;
            next = curr->next;

            if (curr->completion_time < new_min) new_min = curr->completion_time;
        }

        aq->min_activation = new_min;
    }

    ac->min_activation = INFINITY;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if (ac->inactive_patients[i]->min_activation < ac->min_activation) {
            ac->min_activation = ac->inactive_patients[i]->min_activation;
        }
    }
}

in_activation * removeInactivePatient(int idx, activation_queue *aq, activation_center *ac) {

    in_activation *prev = NULL, *current = aq->head, *next = current->next, *removed;

    if (idx == 0) {
        POP_NODE(current, next)
        if (next == aq->tail) aq->tail = NULL;
        removed = next;
    } else {
        REMOVE_MID_NODE(idx, prev, current, next)
        removed = current;
    }

    aq->number--;
    ac->total_number--;

    updateMinActivation(aq, ac);

    return removed;
}

void addActivePatientToLost(event_list *events, sim_time *t, patient *p, patients_lost *lost_queue, LOSS_REASON reason) {

    PRIORITY pr = p->priority;
    BLOOD_TYPE bt = p->bt;
    patient_queue_blood_type *pbt_queue = events->patient_arrival.blood_type_queues[bt];

    if (pbt_queue->number == 0) {
#ifdef AUDIT
        printf("patient queue with blood type %d is now empty -"
               "deactivating living donor arrival event\n", p->bt);
#endif
        events->living_donor_completion[bt] = INFINITY;
    }

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
        lost_queue->reneging_time[bt][pr] = getPatientRenege(bt, pr, active, t->current);
        lost_queue->death_time[bt][pr] = getPatientDeath(bt, pr, active, t->current);
    }
}

void addInactivePatientToLost(event_list *events, sim_time *t, patient *p, patients_lost *lost_queue, LOSS_REASON reason) {

    BLOOD_TYPE bt = p->bt;
    activation_queue *aq = events->activation_arrival.inactive_patients[bt];

    /* increment loss */
    if (reason == death) {
        lost_queue->number_dead[bt][2]++;
    } else {
        lost_queue->number_renege[bt][2]++;
    }

    /* Check if there are other patients in queue to eventually deactivate reneging and death events */
    if (aq->number == 0) {
        lost_queue->reneging_time[bt][2] = INFINITY;
        lost_queue->death_time[bt][2] = INFINITY;
    } else {
        // generate next loss time (death and renege)
        lost_queue->reneging_time[bt][2] = getPatientRenege(bt, none, inactive, t->current);
        lost_queue->death_time[bt][2] = getPatientDeath(bt, none, inactive, t->current);
    }
}

void activePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr) {
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
        addActivePatientToLost(events, t, p, lost, reason);
#ifdef AUDIT
        printf("A patient has left the queue with priority %s with reason %d\n", pr_to_str[priority], reason);
#endif
    }
}

void inactivePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt){
    patients_lost *lost = &events->patients_loss;
    activation_center *ac = &events->activation_arrival;
    activation_queue *aq = ac->inactive_patients[bt];
    int index, max_number = (int) aq->number;
    patient *p;

    // choose a random job or remove the oldest one
    if (reason == death) {
        index = 0; /* remove the oldest one */
    } else {
        new_srand(time(NULL));
        index = new_rand(max_number); /* choose a random patient */
    }

    in_activation *act = removeInactivePatient(index, aq, ac);
    if (act != NULL) {
        p = act->patient;
        addInactivePatientToLost(events, t, p, lost, reason);
    }

    free(act);
}

/**
 * Manages the event of a patient renege due to death or other reasons
 * */
void handlePatientLoss(event_list *events, sim_time *t, LOSS_REASON reason, BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt) {

    t->last[patient_loss] = t->current;

    if (pt == active) {
        activePatientLoss(events, t, reason, bt, pr);
    } else {
        inactivePatientLoss(events, t, reason, bt);
    }
}