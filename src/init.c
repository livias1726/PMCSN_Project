#include "headers/init.h"
#include "headers/sim.h"
#include "headers/utils.h"

/**
 * Functions to initialize structs during execution
 */

patient_waiting_list initializeWaitingList() {
    patient_waiting_list waiting_list = {0};
    patient_queue_blood_type *bt_queue;
    patient_queue_priority *pr_queue;

    int i,j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waiting_list.blood_type_queues[i] = calloc(1, sizeof(patient_queue_blood_type));
        MALLOC_HANDLER(waiting_list.blood_type_queues[i])

        bt_queue = waiting_list.blood_type_queues[i];
        bt_queue->bt = i;

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            bt_queue->priority_queue[j] = calloc(1,sizeof(patient_queue_priority));
            MALLOC_HANDLER(bt_queue->priority_queue[j])
            pr_queue = bt_queue->priority_queue[j];

            pr_queue->priority = j;

            pr_queue->queue = calloc(1,sizeof(patient));
            MALLOC_HANDLER(pr_queue->queue)
            pr_queue->queue->bt = i;
            pr_queue->queue->priority = none;
        }
    }

    return waiting_list;
}

organ_bank initializeOrganBank() {
    organ_bank organ_bank = {0};

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organ_bank.queues[i] = calloc(1, sizeof(organ_queue));
        MALLOC_HANDLER(organ_bank.queues[i])
        organ_bank.queues[i]->bt = i;

        organ_bank.queues[i]->queue = calloc(1, sizeof(organ));
        MALLOC_HANDLER(organ_bank.queues[i]->queue)
        organ_bank.queues[i]->queue->bt = i;
    }

    return organ_bank;
}

transplant_center initializeTransplantCenter() {

    transplant_center transplant_center = {0};

    transplant_center.transplanted_patients = calloc(1, sizeof(in_transplant));
    MALLOC_HANDLER(transplant_center.transplanted_patients)

    transplant_center.min_transplant = INFINITY;

    return transplant_center;
}

activation_center initializeActivationCenter() {

    activation_center activation_center = {0};

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        activation_center.inactive_patients[i] = calloc(1, sizeof(activation_queue));
        MALLOC_HANDLER(activation_center.inactive_patients[i])
        activation_center.inactive_patients[i]->bt = i;

        activation_center.inactive_patients[i]->head = calloc(1, sizeof(in_activation));
        MALLOC_HANDLER(activation_center.inactive_patients[i]->head)

        activation_center.inactive_patients[i]->min_activation = INFINITY;
    }

    activation_center.min_activation = INFINITY;

    return activation_center;
}

organs_expired initializeOrgansExpiredQueue() {
    organs_expired organs_expired;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organs_expired.num_renege[i] = 0.0;
        organs_expired.reneging_time[i] = -1;
    }

    return organs_expired;
}

patients_lost initializePatientLostQueue() {
    patients_lost patients_lost;

    int i,j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            patients_lost.number_dead[i][j] = 0.0;
            patients_lost.death_time[i][j] = -1;
            patients_lost.number_renege[i][j] = 0.0;
            patients_lost.reneging_time[i][j] = -1;
        }
    }

    return patients_lost;
}

matched * newMatched(patient p, organ o){

    matched *m = malloc(sizeof(matched));
    MALLOC_HANDLER(m)

    m->patient = p;
    m->organ = o;

    return m;
}

in_transplant *newTransplant(matched *m) {

    in_transplant *in_tr = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(in_tr)

    in_tr->matched = m;
    in_tr->next = NULL;

    return in_tr;
}

in_activation * newInactive(patient *patient){

    in_activation *inactive = malloc(sizeof(in_activation));
    MALLOC_HANDLER(inactive)

    inactive->patient = patient;
    inactive->next = NULL;

    return inactive;
}

patient *newPatient(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt) {

    patient *new = malloc(sizeof(patient));
    MALLOC_HANDLER(new)

    new->priority = pr;
    new->bt = bt;
    new->type = pt;
    new->start_time = 0;
    new->next = NULL;

    return new;
}

organ *newOrgan(BLOOD_TYPE bt, DONOR_TYPE dt) {

    organ *new = malloc(sizeof(organ));
    MALLOC_HANDLER(new)

    new->bt = bt;
    new->dt = dt;
    new->next = NULL;

    return new;
}

event_list* initializeEventList() {

    event_list *events = malloc(sizeof(event_list));
    MALLOC_HANDLER(events)

    events->organ_arrival = initializeOrganBank();
    events->patient_arrival = initializeWaitingList();
    events->activation_arrival = initializeActivationCenter();
    events->transplant_arrival = initializeTransplantCenter();
    events->organs_loss = initializeOrgansExpiredQueue();
    events->patients_loss = initializePatientLostQueue();

    return events;
}

sim_time initializeTime() {
    sim_time t;
    t.current = 0.0;
    t.next = 0.0;

    for (int i = 0; i < NUM_EVENTS; ++i) {
        t.last[i] = -1;
    }

    return t;
}

void initializeEventTime(event_list* events) {
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {

        events->organ_arrival.inter_arrival_time[i] = getOrganArrival(i, deceased, START);
        events->organs_loss.reneging_time[i] = INFINITY;
        events->living_donor_completion[i] = INFINITY;
        events->activation_arrival.inter_arrival_time[i] = getPatientArrival(i, none, inactive, START);

        for (j = 0; j < NUM_PRIORITIES; ++j) {

            events->patient_arrival.inter_arrival_time[i][j] = getPatientArrival(i, j, active, START);
            events->patients_loss.reneging_time[i][j] = INFINITY;
            events->patients_loss.death_time[i][j] = INFINITY;

        }

        // inactive loss
        events->patients_loss.reneging_time[i][j] = INFINITY;
        events->patients_loss.death_time[i][j] = INFINITY;
    }
}

stats * initializeStatistics(){

    stats *statistics = malloc(sizeof(stats));
    MALLOC_HANDLER(statistics)

    statistics->wl_stats = calloc(1,sizeof(waiting_list_stats));
    MALLOC_HANDLER(statistics->wl_stats)

    statistics->ob_stats = calloc(1,sizeof(organ_bank_stats));
    MALLOC_HANDLER(statistics->ob_stats)

    statistics->trans_stats = calloc(1,sizeof(transplant_stats));
    MALLOC_HANDLER(statistics->trans_stats)

    statistics->act_stats = calloc(1,sizeof(activation_stats));
    MALLOC_HANDLER(statistics->act_stats)

    return statistics;
}

time_integrated_stats * initializeTimeStatistics(){

    time_integrated_stats *statistics = malloc(sizeof(time_integrated_stats));
    MALLOC_HANDLER(statistics)

    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        statistics->area_bank[i] = calloc(1, sizeof(area));
        MALLOC_HANDLER(statistics->area_bank[i])

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            statistics->area_waiting_list[i][j] = calloc(1, sizeof(area));
            MALLOC_HANDLER(statistics->area_waiting_list[i][j])
        }
    }

    statistics->area_activation = calloc(1, sizeof(area));
    MALLOC_HANDLER(statistics->area_activation)

    statistics->area_transplant = calloc(1, sizeof(area));
    MALLOC_HANDLER(statistics->area_transplant)

    return statistics;
}