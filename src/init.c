#include "headers/utils.h"

// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initializeWaitingList() {
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
            waitingList.globalWaitingTimes[i][j] = 0.0;
        }
    }
    waitingList.total_number = 0.0;
    return waitingList;
}

organ_bank initializeOrganBank() {
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

transplant initializeTransplantCenter() {
    transplant transplantCenter;
    transplantCenter.transplanted_patients = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(transplantCenter.transplanted_patients);
    transplantCenter.transplanted_patients->next=NULL;
    transplantCenter.total_number = 0.0;
    transplantCenter.completed_transplants = 0.0;
    transplantCenter.rejected_transplants = 0.0;
    return transplantCenter;
}

activation initializeActivationCenter() {
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

organs_expired initializeOrgansExpiredQueue() {
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

patients_lost initializePatientLostQueue() {
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

matched * newMatched(patient p, organ o){
    matched *m = malloc(sizeof(matched));
    MALLOC_HANDLER(m)
    m->patient = p;
    m->organ = o;
    m->next = NULL;

    return m;
}

in_transplant * newTransplant(matched *m, double so){
    in_transplant *in_tr = malloc(sizeof(in_transplant));
    MALLOC_HANDLER(in_tr)
    in_tr->matched = m;
    in_tr->next = NULL;
    in_tr->serverOffset = so;

    return in_tr;
}

in_activation * newInactive(patient *patient, double server_offset){
    in_activation *inactive = malloc(sizeof(in_activation));
    MALLOC_HANDLER(inactive)
    inactive->patient = patient;
    inactive->next = NULL;
    inactive->serverOffset = server_offset;

    return inactive;
}

patient *newPatient(BLOOD_TYPE bt, PRIORITY pr) {
    patient *new = malloc(sizeof(patient));
    MALLOC_HANDLER(new)
    new->priority = pr;
    new->bt = bt;
    new->is_active = (int) Random() % 2; // FIXME integrate with probability to be inactive and handle it
    new->start_time = -1;
    new->end_time = -1;
    new->next = NULL;
    return new;
}

organ * newOrgan(BLOOD_TYPE bt) {
    organ *new = malloc(sizeof(organ));
    MALLOC_HANDLER(new)
    new->starting_age = ((int) rand() % (24-0+1)) + 0;
    new->bt = bt;
    new->next = NULL;
    return new;
}

event_list initializeEventList() {
    patient_waiting_list waiting_list = initializeWaitingList();
    activation activation_c = initializeActivationCenter();
    organ_bank bank = initializeOrganBank();
    transplant transplant_c = initializeTransplantCenter();
    patients_lost patient_loss = initializePatientLostQueue();
    organs_expired organs_loss = initializeOrgansExpiredQueue();

    event_list eventList;
    eventList.organ_arrival = bank;
    eventList.patient_arrival = waiting_list;
    eventList.activation_arrival = activation_c;
    eventList.transplant_arrival = transplant_c;
    eventList.organs_loss = organs_loss;
    eventList.patients_loss = patient_loss;
    return eventList;
}

sim_time initializeTime() {
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
    events->organ_arrival.interArrivalTime[O] = getOrganArrival(O, START);
    events->organ_arrival.interArrivalTime[A] = getOrganArrival(A, START);
    events->organ_arrival.interArrivalTime[B] = getOrganArrival(B, START);
    events->organ_arrival.interArrivalTime[AB] = getOrganArrival(AB, START);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events->patient_arrival.interArrivalTime[i][j] = getPatientArrival(i, j, START);
        }
    }

    events->organs_loss.reneging_time[O] = INFINITY;
    events->organs_loss.reneging_time[A] = INFINITY;
    events->organs_loss.reneging_time[B] = INFINITY;
    events->organs_loss.reneging_time[AB] = INFINITY;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events->patients_loss.reneging_time[i][j] = INFINITY;
            events->patients_loss.death_time[i][j] = INFINITY;
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

    statistics->area_waiting_list = malloc(sizeof(area));
    statistics->area_activation = malloc(sizeof(area));
    statistics->area_bank = malloc(sizeof(area));
    statistics->area_transplant = malloc(sizeof(area));

    statistics->numTransplants[0] = statistics->numTransplants[1] = 0;

    return statistics;
}