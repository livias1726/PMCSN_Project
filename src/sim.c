#include "headers/sim.h"
#include "headers/stats.h"
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

void infiniteSim(event_list *events, sim_time *t, time_integrated_stats *ti_stats, stats **batches, stats *final_stat,
                 int *num_iterations) {

    double checkpoint = t->current + BATCH_SIZE;           // set first batch;
    bool new_batch;
    int i, iteration = 0;
    t->current = 0;

    while (CLOSE_THE_DOOR(events)) {
        t->next = getMinTime(events);		            // Choose next event selecting minimum time
        updateIntegralsStats(events, t, ti_stats);      // Update integrals stats

        new_batch = t->current > checkpoint;

        if (new_batch){
            //new_batch = t->current < STOP;

            gatherResults(batches[iteration], events);
            computeTimeAveragedStats(batches[iteration], ti_stats, t);
            welford(iteration+1, final_stat, batches[iteration]);
            saveResultsCsv(batches[iteration], true, iteration);

            iteration++;
            checkpoint = t->current + BATCH_SIZE;
            /*

            if (new_batch) {
                iteration++;
                checkpoint = t->current + BATCH_SIZE;
            } else {
                printf("");
            }
            */
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

    gatherResults(final_stat, events); // to update the system state at the end of the simulation
    *num_iterations = iteration;
}

