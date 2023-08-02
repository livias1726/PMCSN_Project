#include "../headers/events.h"

void addOrganToLost(event_list *events, sim_time *t, organ *o) {

    BLOOD_TYPE bt = o->bt;
    organ_bank *bank = &events->organ_arrival;
    organs_expired *exp_queue = &events->organs_loss;

    /* increment loss */
    exp_queue->num_renege[bt]++;

    /* Check if the organ queue is empty to eventually deactivate loss event */
    if (!bank->queues[bt]->organ_available) {
#ifdef AUDIT
        printf("organ queue with blood type %d is now empty - deactivating renege event\n", o->bt);
#endif
        exp_queue->reneging_time[bt] = INFINITY;
    } else {
        // generate next organ loss time
        exp_queue->reneging_time[bt] = getOrganRenege(t->current);
    }

    free(o);
}

/***
 * Function to handle the expiration of an organ in a blood type queue of the organ bank
 */
void handleOrganRenege(event_list *events, sim_time *t, BLOOD_TYPE bt) {

    t->last[organ_renege] = t->current;

    organ_bank *bank = &events->organ_arrival;

    /* Remove the oldest organ in the queue */
    organ *o = removeOrgan(bank->queues[bt], bank);
    addOrganToLost(events, t, o);

    if (!bank->queues[bt]->organ_available) {
        events->organs_loss.reneging_time[bt] = INFINITY;
    } else {
        // Generate next renege time for organ
        events->organs_loss.reneging_time[bt] = getOrganRenege(t->current);
    }
}