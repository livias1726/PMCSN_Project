#include "../headers/events.h"

/**
 * There are many reasons why people might be listed as “temporarily inactive.”
 * Everyone who falls into this category has already been evaluated and accepted by a transplant center,
 * but cannot receive an organ while listed with this status. This status change may be due to a change in one’s
 * current health status (for example, if another illness or infection is active and would jeopardize the transplant),
 * incomplete insurance/financial arrangements or other reasons. At any time, transplant centers can reactivate
 * temporarily inactive status without patients losing priority on the waitlist.
 *
 * https://www.kidney.org/transplantation/transaction/TC/winter14/UNOS_Committee
 * */

in_activation *getNextActivation(double min_completion, activation_center *ac, activation_queue *aq) {
    in_activation *prev = NULL, *curr = aq->head, *next = curr->next, *to_activate = NULL;
    double new_min = INFINITY;

    while (next != NULL) {
        prev = curr;
        curr = next;
        next = curr->next;

        if (curr->completion_time == min_completion) {
            prev->next = next;  /* unlink the node */
            curr->next = NULL;
            if (curr == aq->tail) aq->tail = prev;
            to_activate = curr;
        } else {
            if (curr->completion_time < new_min) new_min = curr->completion_time;
        }
    }

    // update new activation min
    aq->min_activation = new_min;
    ac->min_activation = new_min;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if (ac->inactive_patients[i]->min_activation < ac->min_activation)
            ac->min_activation = ac->inactive_patients[i]->min_activation;
    }

    return to_activate;
}

/**
 * This event triggers the addition of an inactive patient (waiting in the activation center)
 * to the waiting list to receive a transplant.
 * */
void handlePatientActivation(event_list *events, sim_time *t) {
    t->last[activation_completion] = t->current;

    patients_lost *lost_queue = &events->patients_loss;
    activation_center *ac = &events->activation_arrival;

    // get the list where the patient with the min completion time is hosted
    activation_queue *aq;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        aq = ac->inactive_patients[i];
        if (aq->min_activation == t->current) break;
    }

    in_activation *to_activate = getNextActivation(t->current, ac, aq);

    aq->number--;
    ac->total_number--;
    ac->num_completions++;

    /* Check if there are other patients in queue to eventually deactivate reneging and death events */
    if (aq->number == 0) {
        lost_queue->reneging_time[to_activate->patient->bt][2] = INFINITY;
        lost_queue->death_time[to_activate->patient->bt][2] = INFINITY;
    }

    /* get patient to activate and send it to the waiting list with normal priority */
    patient* p = to_activate->patient;
    /*TODO MAYBE if (getCriticalProb() < CRIT_P) {
        // patient gets critical priority
        p->priority = critical;
    }*/
    addToWaitingList(events, t, p);
#ifdef AUDIT
    printf("Patient with blood type %d activated and sent to waiting list\n", p->bt);
#endif
}