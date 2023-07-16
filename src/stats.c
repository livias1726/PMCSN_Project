#include "headers/utils.h"

/**
 * Processes the gathered data to retrieve the system's statistics
 * */
void computeStats(stats* statistics){
    // compute mean for the global waiting times
    double tot_by_queue[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            // get the number of patients per queue that have left the system
            tot_by_queue[i][j] = statistics->numPatientArrivals[i][j] - statistics->numPatients[i][j];
            statistics->meanGlobalWaitingTimePerQueue[i][j] /= tot_by_queue[i][j];
        }
    }
}

/**
 * Retrieves the data to be processed taken during the simulation
 * */
void gatherResults(stats* statistics, event_list *events){
    patient_waiting_list waiting_list = events->patient_arrival;
    organ_bank bank = events->organ_arrival;
    transplant transplant_c = events->transplant_arrival;
    organs_expired organs_expired = events->organs_loss;
    patients_lost patients_lost = events->patients_loss;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        statistics->numOrgans[i] = bank.queues[i]->number;
        statistics->numOrganArrivals[i] = bank.numOrganArrivals[i];
        statistics->numOrganOutdatings[i] = organs_expired.number[i];
        statistics->organsInterarrivalTime[i] = bank.interArrivalTime[i];

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            statistics->numPatients[i][j] = waiting_list.blood_type_queues[i]->priority_queue[j]->number;
            statistics->numPatientArrivals[i][j] = waiting_list.numPatientArrivals[i][j];
            statistics->numDeaths[i][j] = patients_lost.number_dead[i][j];
            statistics->numReneges[i][j] = patients_lost.number_renege[i][j];
            statistics->patientsInterarrivalTime[i][j] = waiting_list.interArrivalTime[i][j];

            statistics->meanGlobalWaitingTimePerQueue[i][j] = waiting_list.globalWaitingTimes[i][j];
        }
    }

    statistics->numTransplants[0] = transplant_c.completed_transplants;
    statistics->numTransplants[1] = transplant_c.rejected_transplants;
}
