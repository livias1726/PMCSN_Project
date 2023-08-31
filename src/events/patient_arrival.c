#include "../headers/events.h"
#include "../headers/init.h"
#include "../headers/utils.h"

// adds inactive patient to the activation center
void addToActivationCenter(event_list *events, sim_time *t, patient *p) {
    t->last[activation_arrival] = t->current;

    BLOOD_TYPE patient_bt = p->bt;
    activation_center *ac = &events->activation_arrival;
    activation_queue *aq = ac->inactive_patients[patient_bt];

    /* add patient to activation center */
    in_activation *new_act = newInactive(p);
    new_act->completion_time = getActivationCompletion(t->current); // generate and change activation time
    in_activation *last = aq->tail;
    APPEND_NEW_NODE(aq->head, aq->tail, last, new_act)

    /* update min activation time*/
    if (new_act->completion_time < aq->min_activation) {
        aq->min_activation = new_act->completion_time;
        if (new_act->completion_time < ac->min_activation) ac->min_activation = new_act->completion_time;
    }

    ac->total_number++;
    aq->number++;
    ac->num_arrivals[p->bt]++;

    p->start_time = t->current;

    /* Check if there is just one patient in queue to eventually activate patient death and renege */
    if (ac->inactive_patients[patient_bt]->number == 1) {
        events->patients_loss.reneging_time[patient_bt][2] = getPatientRenege(patient_bt, none, inactive, t->current);
        events->patients_loss.death_time[patient_bt][2] = getPatientDeath(patient_bt, none, inactive, t->current);
    }

    /* Generate next arrival time of a patient */
    ac->inter_arrival_time[patient_bt] = getPatientArrival(patient_bt, none, inactive, t->current);
}

/***
 * Handler of a new patient's arrival.
 *
 * If the patient is inactive, they will be inserted in the activation center, where they must wait for a condition
 * improvement to be activated and receive a transplant. Otherwise, the patient is active and will be inserted in the
 * wait list to receive a transplant with a priority given by their health conditions.
 */
void handlePatientArrival(event_list *events, sim_time *t, BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt) {
    /* New patient */
    patient* p = newPatient(bt, pr, pt);

    if (pt == inactive) {
        addToActivationCenter(events, t, p);
    } else {
        addToWaitingList(events, t, p);
    }
}

