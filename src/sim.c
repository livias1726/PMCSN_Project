#include "headers/sim.h"
#include "headers/stats.h"
#include "headers/init.h"
#include "headers/utils.h"

double getMinTime(event_list *events) {
    int len = 50, idx = 0;
    double values[len];

    int i,j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        values[idx++] = events->organ_arrival.inter_arrival_time[i];
        values[idx++] = events->organs_loss.reneging_time[i];
        values[idx++] = events->living_donor_completion[i];

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            values[idx++] = events->patient_arrival.inter_arrival_time[i][j];
            values[idx++] = events->patients_loss.reneging_time[i][j];
            values[idx++] = events->patients_loss.death_time[i][j];
        }

        values[idx++] = events->activation_arrival.inter_arrival_time[i];
        values[idx++] = events->patients_loss.reneging_time[i][j];
        values[idx++] = events->patients_loss.death_time[i][j];

    }

    values[idx++] = events->activation_arrival.min_activation;
    values[idx++] = events->transplant_arrival.min_transplant;

    double min;
    GET_MIN(values, idx, min)

    return min;
}

void setupSystemState(event_list *events) {
    int i, j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        events->organ_arrival.num_arrivals[i] = 0;
        events->organs_loss.num_renege[i] = 0;
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            events->patient_arrival.num_arrivals[i][j] = 0;
            events->patients_loss.number_renege[i][j] = 0;
            events->patients_loss.number_dead[i][j] = 0;
        }
    }

    events->transplant_arrival = initializeTransplantCenter();
}

void finiteSim(event_list *events, sim_time *t, time_integrated_stats *ti_stats, stats **batches, stats *final_stat,
               int *num_iterations) {
    /* Choose next event selecting minimum time */
    bool init_state = true;
    double checkpoint;
    int i, iteration = 0;
    t->current = 0;

    while ((events->patient_arrival.inter_arrival_time[0][critical] < STOP)||
    (events->patient_arrival.inter_arrival_time[1][critical] < STOP)||
    (events->patient_arrival.inter_arrival_time[2][critical] < STOP)||
    (events->patient_arrival.inter_arrival_time[3][critical] < STOP)||
    (events->patient_arrival.inter_arrival_time[0][normal] < STOP)||
    (events->patient_arrival.inter_arrival_time[1][normal] < STOP)||
    (events->patient_arrival.inter_arrival_time[2][normal] < STOP)||
    (events->patient_arrival.inter_arrival_time[3][normal] < STOP)||
    (events->patient_arrival.total_number > 0) ||
    (events->transplant_arrival.total_number > 0) ||
    (events->activation_arrival.total_number > 0)) {
        t->next = getMinTime(events);		            // Next event time
        updateIntegralsStats(events, t, ti_stats);      // Update integrals stats
        if (init_state) {
            init_state = false;
            checkpoint = t->current + BATCH_SIZE;           // set first batch
        }
        if ((t->current > checkpoint) && (t->current < STOP)) {
            gatherResults(batches[iteration], events, false);
            computeTimeAveragedStats(batches[iteration], ti_stats, t);
            welford(iteration+1, final_stat, batches[iteration]);
            iteration++;
            checkpoint = t->current + BATCH_SIZE;
        } else if ((t->current > checkpoint) && (t->current >= STOP)){
            // batchone finale
            gatherResults(batches[iteration], events, true);
            computeTimeAveragedStats(batches[iteration], ti_stats, t);
            welford(iteration+1, final_stat, batches[iteration]);
        }
        t->current = t->next;                           // Clock update

        if (t->current == events->activation_arrival.min_activation) {
            handlePatientActivation(events, t);
            continue;
        } else if (t->current == events->transplant_arrival.min_transplant) {
            handleTransplantCompletion(events, t);
            continue;
        }

        for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
            if (t->current == events->organ_arrival.inter_arrival_time[i]) {
                handleOrganArrival(events, t, i, deceased);
                break;
            } else if (t->current == events->living_donor_completion[i]) {
                handleOrganArrival(events, t, i, living);
                break;
            } else if (t->current == events->organs_loss.reneging_time[i]) {
                handleOrganRenege(events, t, i);
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][critical]) {
                handlePatientArrival(events, t, i, critical, active);
                if (events->patient_arrival.inter_arrival_time[i][critical] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->patient_arrival.inter_arrival_time[i][critical] = INFINITY;
                }
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][normal]) {
                handlePatientArrival(events, t, i, normal, active);
                if (events->patient_arrival.inter_arrival_time[i][normal] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->patient_arrival.inter_arrival_time[i][normal] = INFINITY;
                }
                break;
            } else if (t->current == events->activation_arrival.inter_arrival_time[i]) {
                handlePatientArrival(events, t, i, none, inactive);
                if (events->activation_arrival.inter_arrival_time[i] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->activation_arrival.inter_arrival_time[i] = INFINITY;
                }
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical, active);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][normal]) {
                handlePatientLoss(events, t, renege, i, normal, active);
                break;
            } else if (t->current == events->patients_loss.death_time[i][critical]) {
                handlePatientLoss(events, t, death, i, critical, active);
                break;
            } else if (t->current == events->patients_loss.death_time[i][normal]) {
                handlePatientLoss(events, t, death, i, normal, active);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][2]) {
                handlePatientLoss(events, t, renege, i, none, inactive);
                break;
            } else if (t->current == events->patients_loss.death_time[i][2]) {
                handlePatientLoss(events, t, death, i, none, inactive);
                break;
            }
        }
    }

    gatherResults(final_stat, events, 0); // to update the system state at the end of the simulation
    *num_iterations = iteration;
}

