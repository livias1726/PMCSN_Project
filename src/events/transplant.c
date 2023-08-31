#include "../headers/events.h"

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

    // update new transplant min
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
    // FIXME MAYBE bool repeated_transplant = p->repeated_transplant;
    BLOOD_TYPE bt = p->bt;
    PRIORITY pr = p->priority;
    double prob = getRejectionProb();
    double rej;
#ifdef IMPROVEMENT
    rej = (COMPATIBLE(to_transplant->matched->organ.bt, bt)) ? REJECT_P[0] : REJECT_P[1];
#else
    rej = REJECT_P;
#endif

    if (prob < rej) {
        // FIXME MAYBE p->repeated_transplant = true;
        events->transplant_arrival.num_rejections[bt][pr]++;
        addToWaitingList(events, t, p);
#ifdef AUDIT
        printf("Patient with blood type %s was rejected and and sent back to waiting list with priority %s\n",
               bt_to_str[p->bt], pr_to_str[p->priority]);
#endif
    } else {
        // FIXME MAYBE if (p->repeated_transplant == false) events->transplant_arrival.num_completions[bt][pr]++;
        events->transplant_arrival.num_completions[bt][pr]++;
    }

    events->transplant_arrival.total_number--;
    free(to_transplant);
}