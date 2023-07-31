#include "headers/utils.h"

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats);

double getMinTime(event_list *events) {
    int len = 49, idx = 0;
    double timesToCompare[len];

    // 0 - 3
    timesToCompare[idx++] = events->organ_arrival.inter_arrival_time[O];
    timesToCompare[idx++] = events->organ_arrival.inter_arrival_time[A];
    timesToCompare[idx++] = events->organ_arrival.inter_arrival_time[B];
    timesToCompare[idx++] = events->organ_arrival.inter_arrival_time[AB];
    // 4 - 7
    timesToCompare[idx++] = events->living_donor_completion[O];
    timesToCompare[idx++] = events->living_donor_completion[A];
    timesToCompare[idx++] = events->living_donor_completion[B];
    timesToCompare[idx++] = events->living_donor_completion[AB];

    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[O][critical];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[O][active];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[O][inactive];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[A][critical];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[A][active];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[A][inactive];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[B][critical];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[B][active];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[B][inactive];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[AB][critical];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[AB][active];
    timesToCompare[idx++] = events->patient_arrival.inter_arrival_time[AB][inactive];

    timesToCompare[idx++] = events->organs_loss.reneging_time[O];
    timesToCompare[idx++] = events->organs_loss.reneging_time[A];
    timesToCompare[idx++] = events->organs_loss.reneging_time[B];
    timesToCompare[idx++] = events->organs_loss.reneging_time[AB];

    timesToCompare[idx++] = events->patients_loss.reneging_time[O][critical];
    timesToCompare[idx++] = events->patients_loss.reneging_time[O][active];
    timesToCompare[idx++] = events->patients_loss.reneging_time[O][inactive];
    timesToCompare[idx++] = events->patients_loss.reneging_time[A][critical];
    timesToCompare[idx++] = events->patients_loss.reneging_time[A][active];
    timesToCompare[idx++] = events->patients_loss.reneging_time[A][inactive];
    timesToCompare[idx++] = events->patients_loss.reneging_time[B][critical];
    timesToCompare[idx++] = events->patients_loss.reneging_time[B][active];
    timesToCompare[idx++] = events->patients_loss.reneging_time[B][inactive];
    timesToCompare[idx++] = events->patients_loss.reneging_time[AB][critical];
    timesToCompare[idx++] = events->patients_loss.reneging_time[AB][active];
    timesToCompare[idx++] = events->patients_loss.reneging_time[AB][inactive];

    timesToCompare[idx++] = events->patients_loss.death_time[O][critical];
    timesToCompare[idx++] = events->patients_loss.death_time[O][active];
    timesToCompare[idx++] = events->patients_loss.death_time[O][inactive];
    timesToCompare[idx++] = events->patients_loss.death_time[A][critical];
    timesToCompare[idx++] = events->patients_loss.death_time[A][active];
    timesToCompare[idx++] = events->patients_loss.death_time[A][inactive];
    timesToCompare[idx++] = events->patients_loss.death_time[B][critical];
    timesToCompare[idx++] = events->patients_loss.death_time[B][active];
    timesToCompare[idx++] = events->patients_loss.death_time[B][inactive];
    timesToCompare[idx++] = events->patients_loss.death_time[AB][critical];
    timesToCompare[idx++] = events->patients_loss.death_time[AB][active];
    timesToCompare[idx++] = events->patients_loss.death_time[AB][inactive];

    timesToCompare[idx++] = events->transplant_arrival.min_transplant;

    double min;
    GET_SMALLEST(timesToCompare, len, min)

    return min;
}

void setupSystemState(event_list *events) {
    int i, j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        events->organ_arrival.num_arrivals[i] = 0;
        events->organs_loss.number[i] = 0;
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            events->patient_arrival.num_arrivals[i][j] = 0;
            events->patients_loss.number_renege[i][j] = 0;
            events->patients_loss.number_dead[i][j] = 0;
        }
    }

    events->transplant_arrival = initializeTransplantCenter();
}

/**
 * Welford's one-pass algorithm to compute the mean and the standard deviation for each statistic
 * */
void welford(int iter, stats *stat, stats *batch){
    double diff;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        WELFORD(diff, batch->ob_stats->avg_interarrival_time[i],stat->ob_stats->avg_interarrival_time[i],stat->ob_stats->std_interarrival_time[i],iter)
        WELFORD(diff, batch->ob_stats->avg_in_queue[i],stat->ob_stats->avg_in_queue[i],stat->ob_stats->std_in_queue[i],iter)

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            // Waiting list
            WELFORD(diff, batch->wl_stats->avg_interarrival_time[i][j], stat->wl_stats->avg_interarrival_time[i][j],stat->wl_stats->std_interarrival_time[i][j], iter)
            WELFORD(diff, batch->wl_stats->avg_wait[i][j],stat->wl_stats->avg_wait[i][j],stat->wl_stats->std_wait[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_delay[i][j],stat->wl_stats->avg_delay[i][j],stat->wl_stats->std_delay[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_service[i][j],stat->wl_stats->avg_service[i][j],stat->wl_stats->std_service[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_in_node[i][j],stat->wl_stats->avg_in_node[i][j],stat->wl_stats->std_in_node[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_in_queue[i][j],stat->wl_stats->avg_in_queue[i][j],stat->wl_stats->std_in_queue[i][j],iter)
            WELFORD(diff, batch->wl_stats->utilization[i][j],stat->wl_stats->utilization[i][j],stat->wl_stats->std_utilization[i][j],iter)
        }
    }

    // Transplant center
    WELFORD(diff, batch->trans_stats->avg_in_node,stat->trans_stats->avg_in_node,stat->trans_stats->std_in_node,iter)
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
    (events->patient_arrival.inter_arrival_time[0][active] < STOP)||
    (events->patient_arrival.inter_arrival_time[1][active] < STOP)||
    (events->patient_arrival.inter_arrival_time[2][active] < STOP)||
    (events->patient_arrival.inter_arrival_time[3][active] < STOP)||
    (events->patient_arrival.inter_arrival_time[0][inactive] < STOP)||
    (events->patient_arrival.inter_arrival_time[1][inactive] < STOP)||
    (events->patient_arrival.inter_arrival_time[2][inactive] < STOP)||
    (events->patient_arrival.inter_arrival_time[3][inactive] < STOP) ||
    (events->patient_arrival.total_number > 0)) {
        t->next = getMinTime(events);		            // Next event time
        // check initialization phase
        if (t->current > INIT) {
            updateIntegralsStats(events, t, ti_stats);  // Update integrals stats

            if (init_state) { // first iteration after initialization phase
                setupSystemState(events);
                init_state = false;
                checkpoint = t->current + BATCH_SIZE;   // set first batch
            } else if (t->current > checkpoint) {
                gatherResults(batches[iteration], events);
                computeTimeAveragedStats(batches[iteration], ti_stats, t);
                welford(iteration+1, final_stat, batches[iteration]);
                iteration++;
                checkpoint = t->current + BATCH_SIZE;
            }
        }
        t->current = t->next;                           // Clock update

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
                handlePatientArrival(events, t, i, critical);
                if (events->patient_arrival.inter_arrival_time[i][critical] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->patient_arrival.inter_arrival_time[i][critical] = INFINITY;
                }
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][active]) {
                handlePatientArrival(events, t, i, active);
                if (events->patient_arrival.inter_arrival_time[i][active] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->patient_arrival.inter_arrival_time[i][active] = INFINITY;
                }
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][inactive]) {
                handlePatientArrival(events, t, i, inactive);
                if (events->patient_arrival.inter_arrival_time[i][inactive] > STOP) {
                    t->last[patient_arrival] = t->current;
                    events->patient_arrival.inter_arrival_time[i][inactive] = INFINITY;
                }
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][active]) {
                handlePatientLoss(events, t, renege, i, active);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][inactive]) {
                handlePatientLoss(events, t, renege, i, inactive);
                break;
            } else if (t->current == events->patients_loss.death_time[i][critical]) {
                handlePatientLoss(events, t, death, i, critical);
                break;
            } else if (t->current == events->patients_loss.death_time[i][active]) {
                handlePatientLoss(events, t, death, i, active);
                break;
            } else if (t->current == events->patients_loss.death_time[i][inactive]) {
                handlePatientLoss(events, t, death, i, inactive);
                break;
            } else if (t->current == events->transplant_arrival.min_transplant) {
                handleTransplantCompletion(events, t);
                break;
            }
        }
    }

    gatherResults(final_stat, events); // to update the system state at the end of the simulation
    *num_iterations = iteration;
}

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats) {
    double number_trans = events->transplant_arrival.total_number;
    double number_wl, number_bank;
    double diff = t->next - t->current;

    int i,j;

    /* Update waiting list integrals */
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {

        /* Update organ bank integrals */
        number_bank = events->organ_arrival.queues[i]->number;
        ti_stats->area_bank[i]->node += diff * (number_bank+1);
        ti_stats->area_bank[i]->queue += diff * (number_bank);
        ti_stats->area_bank[i]->service += diff;

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            number_wl = events->patient_arrival.blood_type_queues[i]->priority_queue[j]->number;
            ti_stats->area_waiting_list[i][j]->node += diff * (number_wl+1);
            ti_stats->area_waiting_list[i][j]->queue += diff * (number_wl);
            ti_stats->area_waiting_list[i][j]->service += diff;
        }
    }

    /* Update transplant center integrals */
    ti_stats->area_transplant->node += diff * number_trans;
    ti_stats->area_transplant->service += diff * number_trans;
}
