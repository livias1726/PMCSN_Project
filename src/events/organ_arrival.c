#include "../headers/events.h"
#include "../headers/init.h"

/***
 * Handler of a new organ's arrival.
 *
 * Checks the availability of a compatible patient in waiting list and eventually triggers a matching event.
 * If no compatible patient is available, adds the organ in the specific blood type queue of the organ bank
 */
void handleOrganArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, DONOR_TYPE dt) {

    t->last[organ_arrival] = t->current;

    organ *o;
    organ_bank *bank = &events->organ_arrival;

    /* Generate next arrival time of an organ */
    if (dt == living) {
        // living donor organ
        events->living_donor_completion[bt] = getOrganArrival(bt, living, t->current);
    } else {
        // deceased donor organ
        bank->inter_arrival_time[bt] = getOrganArrival(bt, deceased, t->current);
    }

    o = newOrgan(bt, dt);

    /* Increment arrivals */
    bank->num_arrivals[bt][dt]++;

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