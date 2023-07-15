#include "headers/utils.h"

// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initialize_waiting_list() {
    patient_waiting_list waitingList;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waitingList.blood_type_queues[i] = malloc(sizeof(patient_queue_blood_type));
        MALLOC_HANDLER(waitingList.blood_type_queues[i])

        waitingList.blood_type_queues[i]->bt = i;
        waitingList.blood_type_queues[i]->number = 0.0;

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            waitingList.blood_type_queues[i]->priority_queue[j] = malloc(sizeof(patient_queue_priority));
            MALLOC_HANDLER(waitingList.blood_type_queues[i]->priority_queue[j])

            waitingList.blood_type_queues[i]->priority_queue[j]->number = 0.0;
            waitingList.blood_type_queues[i]->priority_queue[j]->priority = j;

            waitingList.blood_type_queues[i]->priority_queue[j]->queue = malloc(sizeof(patient));
            MALLOC_HANDLER(waitingList.blood_type_queues[i]->priority_queue[j]->queue)
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->bt = i;

            waitingList.blood_type_queues[i]->priority_queue[j]->queue->priority = none;
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->is_active = false;
            waitingList.blood_type_queues[i]->priority_queue[j]->queue->next = NULL;

            waitingList.numPatientArrivals[i][j] = 0.0;
        }
    }
    waitingList.total_number = 0.0;
    return waitingList;
}

organ_bank initialize_organ_bank() {
    organ_bank organBank;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organBank.queues[i] = malloc(sizeof(organ_queue));
        MALLOC_HANDLER(organBank.queues[i])
        organBank.queues[i]->bt = i;
        organBank.queues[i]->number = 0.0;

        organBank.queues[i]->queue = malloc(sizeof(organ));
        MALLOC_HANDLER(organBank.queues[i]->queue)
        organBank.queues[i]->queue->bt = i;
        organBank.queues[i]->queue->next = NULL;

        organBank.numOrganArrivals[i] = 0.0;
    }

    organBank.total_number = 0.0;
    return organBank;
}

transplant initialize_transplant_center() {
    transplant transplantCenter;
    transplantCenter.transplanted_patients = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(transplantCenter.transplanted_patients);
    transplantCenter.transplanted_patients->next=NULL;
    transplantCenter.total_number = 0.0;
    transplantCenter.completed_transplants = 0.0;
    transplantCenter.rejected_transplants = 0.0;
    return transplantCenter;
}

activation initialize_activation_center() {
    activation activationCenter;
    activationCenter.inactive_patients = malloc(sizeof(in_activation));
    MALLOC_HANDLER(activationCenter.inactive_patients)
    // init head
    activationCenter.inactive_patients->next = NULL;
    // init patients head
    activationCenter.inactive_patients->patient = malloc(sizeof(patient));
    MALLOC_HANDLER(activationCenter.inactive_patients->patient)
    activationCenter.inactive_patients->patient->next = NULL;

    activationCenter.total_number = 0.0;
    return activationCenter;
}

organs_expired initialize_organs_expired_queue() {
    organs_expired organsExpired;
    organsExpired.queue = malloc(sizeof(organ));
    MALLOC_HANDLER(organsExpired.queue)
    // head init
    organsExpired.queue->next = NULL;
    organsExpired.queue->starting_age = -1;

    organsExpired.number[O] = 0.0;
    organsExpired.number[A] = 0.0;
    organsExpired.number[B] = 0.0;
    organsExpired.number[AB] = 0.0;
    organsExpired.reneging_time[O] = -1;
    organsExpired.reneging_time[A] = -1;
    organsExpired.reneging_time[B] = -1;
    organsExpired.reneging_time[AB] = -1;

    return organsExpired;
}

patients_lost initialize_patient_lost_queue() {
    patients_lost patientsLost;
    patientsLost.queue = malloc(sizeof(patient));
    MALLOC_HANDLER(patientsLost.queue)
    patientsLost.queue->next = NULL;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            patientsLost.number_dead[i][j] = 0.0;
            patientsLost.death_time[i][j] = -1;
            patientsLost.number_renege[i][j] = 0.0;
            patientsLost.reneging_time[i][j] = -1;
        }
    }
    return patientsLost;
}

matched * new_matched(patient p, organ o){
    matched *m = malloc(sizeof(matched));
    MALLOC_HANDLER(m)
    m->patient = p;
    m->organ = o;
    m->next = NULL;

    return m;
}

in_transplant * new_transplant(matched *m, double so){
    in_transplant *in_tr = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(in_tr)
    in_tr->matched = m;
    in_tr->next = NULL;
    in_tr->serverOffset = so;

    return in_tr;
}

in_activation * new_inactive(patient *p, double so){
    in_activation *inactive = malloc(sizeof(in_activation));
    MALLOC_HANDLER(inactive)
    inactive->patient = p;
    inactive->next = NULL;
    inactive->serverOffset = so;

    return inactive;
}

patient * new_patient(BLOOD_TYPE bt, PRIORITY pr) {
    patient *new = malloc(sizeof(patient));
    MALLOC_HANDLER(new)
    new->priority = pr;
    new->bt = bt;
    new->is_active = (int) Random() % 2; // FIXME integrate with probability to be inactive and handle it
    new->next = NULL;
    return new;
}

organ * new_organ(BLOOD_TYPE bt) {
    organ *new = malloc(sizeof(organ));
    MALLOC_HANDLER(new)
    new->starting_age = ((int) rand() % (24-0+1)) + 0;
    new->bt = bt;
    new->next = NULL;
    return new;
}

event_list initialize_event_list() {
    patient_waiting_list waiting_list = initialize_waiting_list();
    activation activation_c = initialize_activation_center();
    organ_bank bank = initialize_organ_bank();
    transplant transplant_c = initialize_transplant_center();
    patients_lost patient_loss = initialize_patient_lost_queue();
    organs_expired organs_loss = initialize_organs_expired_queue();

    event_list eventList;
    eventList.organArrival = bank;
    eventList.patientArrival = waiting_list;
    eventList.activationArrival = activation_c;
    eventList.transplantArrival = transplant_c;
    eventList.organsLoss = organs_loss;
    eventList.patientsLoss = patient_loss;
    return eventList;
}

sim_time initialize_time() {
    sim_time t;
    t.current = 0.0;
    t.next = 0.0;

    //loop unroll
    t.last[0] = 0.0;
    t.last[1] = 0.0;
    t.last[2] = 0.0;
    t.last[3] = 0.0;
    t.last[4] = 0.0;

    return t;
}

void initializeEventTime(event_list* events) {
    events->organArrival.interArrivalTime[O] = getOrganArrival(O, START);
    events->organArrival.interArrivalTime[A] = getOrganArrival(A, START);
    events->organArrival.interArrivalTime[B] = getOrganArrival(B, START);
    events->organArrival.interArrivalTime[AB] = getOrganArrival(AB, START);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events->patientArrival.interArrivalTime[i][j] = getPatientArrival(i, j, START);
        }
    }

    events->organsLoss.reneging_time[O] = INFINITY;
    events->organsLoss.reneging_time[A] = INFINITY;
    events->organsLoss.reneging_time[B] = INFINITY;
    events->organsLoss.reneging_time[AB] = INFINITY;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events->patientsLoss.reneging_time[i][j] = INFINITY;
            events->patientsLoss.death_time[i][j] = INFINITY;
        }
    }
}

stats * initializeStatistics(){

    stats *statistics = malloc(sizeof(stats));

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        statistics->numOrganArrivals[i] = 0;
        statistics->numOrganOutdatings[i] = 0;
        statistics->numOrgans[i] = 0;

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            statistics->numPatientArrivals[i][j] = 0;
            statistics->numDeaths[i][j] = 0;
            statistics->numReneges[i][j] = 0;
            statistics->numPatients[i][j] = 0;
        }
    }

    statistics->numTransplants[0] = statistics->numTransplants[1] = 0;

    return statistics;
}