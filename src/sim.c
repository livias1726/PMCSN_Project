#include "headers/utils.h"

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats);

double getMinTime(event_list *events) {
    int len = 50;

    double timesToCompare[len];
    timesToCompare[0] = events->organ_arrival.inter_arrival_time[O];
    timesToCompare[1] = events->organ_arrival.inter_arrival_time[A];
    timesToCompare[2] = events->organ_arrival.inter_arrival_time[B];
    timesToCompare[3] = events->organ_arrival.inter_arrival_time[AB];
    timesToCompare[4] = events->patient_arrival.inter_arrival_time[O][critical];
    timesToCompare[5] = events->patient_arrival.inter_arrival_time[O][normal];
    timesToCompare[6] = events->patient_arrival.inter_arrival_time[O][low];
    timesToCompare[7] = events->patient_arrival.inter_arrival_time[A][critical];
    timesToCompare[8] = events->patient_arrival.inter_arrival_time[A][normal];
    timesToCompare[9] = events->patient_arrival.inter_arrival_time[A][low];
    timesToCompare[10] = events->patient_arrival.inter_arrival_time[B][critical];
    timesToCompare[11] = events->patient_arrival.inter_arrival_time[B][normal];
    timesToCompare[12] = events->patient_arrival.inter_arrival_time[B][low];
    timesToCompare[13] = events->patient_arrival.inter_arrival_time[AB][critical];
    timesToCompare[14] = events->patient_arrival.inter_arrival_time[AB][normal];
    timesToCompare[15] = events->patient_arrival.inter_arrival_time[AB][low];
    timesToCompare[16] = events->organs_loss.reneging_time[O];
    timesToCompare[17] = events->organs_loss.reneging_time[A];
    timesToCompare[18] = events->organs_loss.reneging_time[B];
    timesToCompare[19] = events->organs_loss.reneging_time[AB];
    timesToCompare[20] = events->patients_loss.reneging_time[O][critical];
    timesToCompare[21] = events->patients_loss.reneging_time[O][normal];
    timesToCompare[22] = events->patients_loss.reneging_time[O][low];
    timesToCompare[23] = events->patients_loss.reneging_time[A][critical];
    timesToCompare[24] = events->patients_loss.reneging_time[A][normal];
    timesToCompare[25] = events->patients_loss.reneging_time[A][low];
    timesToCompare[26] = events->patients_loss.reneging_time[B][critical];
    timesToCompare[27] = events->patients_loss.reneging_time[B][normal];
    timesToCompare[28] = events->patients_loss.reneging_time[B][low];
    timesToCompare[29] = events->patients_loss.reneging_time[AB][critical];
    timesToCompare[30] = events->patients_loss.reneging_time[AB][normal];
    timesToCompare[31] = events->patients_loss.reneging_time[AB][low];
    timesToCompare[32] = events->patients_loss.death_time[O][critical];
    timesToCompare[33] = events->patients_loss.death_time[O][normal];
    timesToCompare[34] = events->patients_loss.death_time[O][low];
    timesToCompare[35] = events->patients_loss.death_time[A][critical];
    timesToCompare[36] = events->patients_loss.death_time[A][normal];
    timesToCompare[37] = events->patients_loss.death_time[A][low];
    timesToCompare[38] = events->patients_loss.death_time[B][critical];
    timesToCompare[39] = events->patients_loss.death_time[B][normal];
    timesToCompare[40] = events->patients_loss.death_time[B][low];
    timesToCompare[41] = events->patients_loss.death_time[AB][critical];
    timesToCompare[42] = events->patients_loss.death_time[AB][normal];
    timesToCompare[43] = events->patients_loss.death_time[AB][low];
    timesToCompare[44] = getMinActivation(events->activation_arrival.inactive_patients);
    timesToCompare[45] = getMinTransplant(events->transplant_arrival.transplanted_patients);
    timesToCompare[46] = events->living_donor_completion[O];
    timesToCompare[47] = events->living_donor_completion[A];
    timesToCompare[48] = events->living_donor_completion[B];
    timesToCompare[49] = events->living_donor_completion[AB];

    double min;
    GET_SMALLEST(timesToCompare, len, min)

    return min;
}

double getMinTransplant(in_transplant *transplanted) {
    double min, tmp;

    if (transplanted->next == NULL) return INFINITY;

    min = transplanted->next->completionTime;
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        tmp = transplanted->completionTime;
        if (tmp < min) min = tmp;
    }
    return min;
}

double getMinActivation(in_activation *inactive) {
    double min, tmp;

    if (inactive->next == NULL) return INFINITY;

    min = inactive->next->completionTime;
    while (inactive->next != NULL) {
        inactive = inactive->next;
        tmp = inactive->completionTime;
        if (tmp < min) min = tmp;
    }
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

    // Activation center
    WELFORD(diff, batch->act_stats->avg_in_node,stat->act_stats->avg_in_node,stat->act_stats->std_in_node,iter)
    WELFORD(diff, batch->act_stats->avg_delay,stat->act_stats->avg_delay,stat->act_stats->std_delay,iter)

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
    int iteration = 0;
    t->current = 0;

    while (t->current < STOP) {
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
                computeTimeAveragedStats2(batches[iteration], ti_stats, t);
                welford(iteration+1, final_stat, batches[iteration]);
                iteration++;
                checkpoint = t->current + BATCH_SIZE;
            }
        }
        t->current = t->next;                           // Clock update

        for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
            if (t->current == events->organ_arrival.inter_arrival_time[i]) {
                handleOrganArrival(events, t, i, false);
                break;
            } else if (t->current == events->living_donor_completion[i]) {
                handleOrganArrival(events, t, i, true);
                break;
            } else if (t->current == events->organs_loss.reneging_time[i]) {
                handleOrganRenege(events, t, i);
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][critical]) {
                handlePatientArrival(events, t, i, critical);
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][normal]) {
                handlePatientArrival(events, t, i, normal);
                break;
            } else if (t->current == events->patient_arrival.inter_arrival_time[i][low]) {
                handlePatientArrival(events, t, i, low);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][normal]) {
                handlePatientLoss(events, t, renege, i, normal);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][low]) {
                handlePatientLoss(events, t, renege, i, low);
                break;
            } else if (t->current == events->patients_loss.death_time[i][critical]) {
                handlePatientLoss(events, t, death, i, critical);
                break;
            } else if (t->current == events->patients_loss.death_time[i][normal]) {
                handlePatientLoss(events, t, death, i, normal);
                break;
            } else if (t->current == events->patients_loss.death_time[i][low]) {
                handlePatientLoss(events, t, death, i, low);
                break;
            } else if (t->current == getMinActivation(events->activation_arrival.inactive_patients)) {
                handlePatientActivation(events, t);
                break;
            } else if (t->current == getMinTransplant(events->transplant_arrival.transplanted_patients)) {
                handleTransplantCompletion(events, t);
                break;
            }
        }
    }

    gatherResults(final_stat, events); // to update the system state at the end of the simulation
    *num_iterations = iteration;
}

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats) {
    double number_active = events->activation_arrival.total_number;
    double number_trans = events->transplant_arrival.total_number;
    double number_wl, number_bank;
    double diff = t->next - t->current;

    int i,j;

    //TODO: maybe add other loss centers?

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

    /* Update activation_center center integrals */
    ti_stats->area_activation->node += diff * number_active;
    ti_stats->area_activation->service += diff * number_active;

    /* Update transplant center integrals */
    ti_stats->area_transplant->node += diff * number_trans;
    ti_stats->area_transplant->service += diff * number_trans;
}
