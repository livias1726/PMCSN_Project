#include "../headers/events.h"
#include "../headers/utils.h"
#include "../headers/sim.h"

// always pop organ from the head of the queue
organ * removeOrgan(organ_queue *queue, organ_bank *bank) {

    organ *head = queue->queue, *first = head->next;
    POP_NODE(head,first)
    if (first == queue->last_organ) queue->last_organ = NULL;

    if (queue->number > 0) queue->number--;
    if (bank->total_number > 0) bank->total_number--;
    if (queue->number == 0) {
        queue->organ_available = false;
#ifdef AUDIT
        printf("No organs left in queue with blood type %s\n", bt_to_str[queue->bt]);
#endif
    }

    return first;
}

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

// adds the patient to a specific queue with a specific priority
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

    /* set patient arrival time: the period of time eventually spent in activation is added */
    p->start_time += curr_time;

    /* add new patient */
    patient *last = pp_queue->last;
    APPEND_NEW_NODE(pp_queue->queue, pp_queue->last, last, p)

    /* Check if there is just one patient in queue to eventually activate patient death and renege */
    if (pp_queue->number == 1) {
        events->patients_loss.reneging_time[bt][pr] = getPatientRenege(bt, pr, active, curr_time);
        events->patients_loss.death_time[bt][pr] = getPatientDeath(bt, pr, active, curr_time);
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

// adds an organ to the organ queue with a specific blood type
void addOrganToQueue(event_list *events, sim_time *t, organ_queue *o_queue, organ *o) {

    /* Increment organs */
    o_queue->number++;
    if (!o_queue->organ_available) o_queue->organ_available = true;
    (&events->organ_arrival)->total_number++;

    /* Add new organ */
    organ *last = o_queue->last_organ;
    APPEND_NEW_NODE(o_queue->queue, o_queue->last_organ, last, o)

    /* Check if there is just one organ to eventually activate next organ renege */
    BLOOD_TYPE bt = o->bt;
    if (o_queue->number == 1) {
#ifdef AUDIT
        printf("Only one organ in queue - activating renege of organs with blood type %d\n", o->bt);
#endif
        events->organs_loss.reneging_time[bt] = getOrganRenege(t->current);
    }
}

// adds the patient to a specific queue with a specific priority
void addToWaitingList(event_list *events, sim_time* t, patient *p) {
    patient_waiting_list *wl = &events->patient_arrival;
    BLOOD_TYPE bt = p->bt;
    PRIORITY priority = p->priority;

    t->last[patient_arrival] = t->current;

    // Generate next arrival time of a patient
    if (p->type == inactive) {
        p->type = active;
        p->priority = normal;
        priority = p->priority;
    } else {
        if (t->current < STOP) {
            //t->last[patient_arrival] = t->current; //TODO: see what happens if this is outside
            events->patient_arrival.inter_arrival_time[bt][priority] =
                    getPatientArrival(bt, priority, active,t->current);
        }
    }

    // increment arrivals to waiting list only if not counted before
    if (!p->repeated_transplant) wl->num_arrivals[bt][priority]++;

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