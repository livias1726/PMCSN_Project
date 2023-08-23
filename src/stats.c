#include "headers/stats.h"

/**
 * This file contains the implementation for every procedure that handles statistics.
 * */

void computeTimeAveragedStats(stats *stats, time_integrated_stats *ti_stats, sim_time *t) {
    double curr = t->current, population_bank, completion_bank, population_wl, completion;
    area * curr_area;

    waiting_list_stats* wl_stats = stats->wl_stats;
    organ_bank_stats* ob_stats = stats->ob_stats;
    transplant_stats* trans_stats = stats->trans_stats;
    activation_stats* act_stats = stats->act_stats;
    int i,j;

#ifdef AUDIT
    printf("curr area - node - AB crit: %f\n", ti_stats->area_waiting_list[AB][critical]->node);
    printf("curr area - node - AB norm: %f\n", ti_stats->area_waiting_list[AB][normal]->node);
    printf("waiting - node - AB crit: %f\n", ti_stats->area_waiting_list[AB][critical]->node/wl_stats->num_patients_served[AB][critical]);
    printf("waiting - node - AB norm: %f\n\n", ti_stats->area_waiting_list[AB][normal]->node/wl_stats->num_patients_served[AB][normal]);

    printf("curr area - node - O crit: %f\n", ti_stats->area_waiting_list[O][critical]->node);
    printf("curr area - node - O norm: %f\n", ti_stats->area_waiting_list[O][normal]->node);
    /* FIXME: dato che i tassi del numero di pazienti serviti sono diversi per ogni coda in realtà è necessario
     * considerare in numero totale per gruppo sanguigno e non per priorità!*/
    printf("waiting - node - O crit: %f\n", ti_stats->area_waiting_list[O][critical]->node/wl_stats->num_patients_served[O][critical]);
    printf("waiting - node - O norm: %f\n\n", ti_stats->area_waiting_list[O][normal]->node/wl_stats->num_patients_served[O][normal]);
#endif

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        curr_area = ti_stats->area_bank[i];
        population_bank = ob_stats->sum_organ_arrivals[i][deceased];
        completion_bank = ob_stats->sum_organs_completions[i][deceased];

        ob_stats->avg_interarrival_time[i] = (population_bank == 0) ? 0 : t->last[organ_arrival] / population_bank;

        if (completion_bank != 0) {
            ob_stats->avg_wait[i][deceased] = curr_area->node / completion_bank;
            ob_stats->avg_delay[i][deceased] = curr_area->queue / completion_bank;
            ob_stats->avg_service[i][deceased] = curr_area->service / completion_bank;
        }

        ob_stats->avg_in_queue[i] = curr_area->queue / curr;

        // Waiting list
        completion = wl_stats->sum_patient_served[i][critical]+wl_stats->sum_patient_served[i][normal];

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            curr_area = ti_stats->area_waiting_list[i][j];
            population_wl = wl_stats->sum_patient_arrivals[i][j];

            wl_stats->avg_interarrival_time[i][j] = (population_wl == 0) ? 0 : t->last[patient_arrival] / population_wl;

            if (completion != 0) {
                wl_stats->avg_wait[i][j] = curr_area->node / completion;
                wl_stats->avg_delay[i][j] = curr_area->queue / completion;
                wl_stats->avg_service[i][j] = curr_area->service / completion;
            }

            wl_stats->avg_in_node[i][j] = curr_area->node / curr;
            wl_stats->avg_in_queue[i][j] = curr_area->queue / curr;
            wl_stats->utilization[i][j] = curr_area->service / curr;
        }

        // Activation
        curr_area = ti_stats->area_activation;
        completion = act_stats->sum_activated[O] +
                     act_stats->sum_activated[A] +
                     act_stats->sum_activated[B] +
                     act_stats->sum_activated[AB];

        act_stats->avg_in_node[i] = curr_area->node / curr;

        if (completion != 0) {
            act_stats->avg_wait[i] = curr_area->node / completion;
            act_stats->avg_delay[i] = curr_area->queue / completion;
            act_stats->avg_service[i] = curr_area->service / completion;
        }

        // Transplant center
        completion = trans_stats->sum_transplanted[i][critical]+trans_stats->sum_transplanted[i][normal];

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            curr_area = ti_stats->area_transplant;

            if (completion != 0) {
                trans_stats->avg_wait[i][j] = curr_area->node / completion;
                trans_stats->avg_delay[i][j] = curr_area->queue / completion;
                trans_stats->avg_service[i][j] = curr_area->service / completion;
            }
        }

        trans_stats->avg_in_node = curr_area->node / curr;
    }
}

/**
 * Retrieves the data taken during the simulation
 * */
static void gatherFirstResults(stats *statistics, event_list *events) {
    patient_waiting_list waiting_list = events->patient_arrival;
    organ_bank bank = events->organ_arrival;
    transplant_center transplant_c = events->transplant_arrival;
    activation_center activation = events->activation_arrival;
    patients_lost p_lost = events->patients_loss;
    organs_expired o_exp = events->organs_loss;

    waiting_list_stats* wl_stats = statistics->wl_stats;
    organ_bank_stats* ob_stats = statistics->ob_stats;
    transplant_stats* t_stats = statistics->trans_stats;
    activation_stats* a_stats = statistics->act_stats;

    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // organ bank
        ob_stats->num_organs_in_queue[i] = bank.queues[i]->number;
        ob_stats->num_organ_outdatings[i] = ob_stats->sum_organ_outdatings[i] = o_exp.num_renege[i];
        for (j = 0; j < NUM_DONOR_TYPES; ++j) {
            ob_stats->num_organ_arrivals[i][j] = ob_stats->sum_organ_arrivals[i][j] = bank.num_arrivals[i][j];
            ob_stats->num_organs_completions[i][j] = ob_stats->sum_organs_completions[i][j] = bank.num_completions[i][j];
        }

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            // waiting list
            wl_stats->num_patients_in_queue[i][j] = waiting_list.blood_type_queues[i]->priority_queue[j]->number;
            wl_stats->num_patient_arrivals[i][j] = wl_stats->sum_patient_arrivals[i][j] = waiting_list.num_arrivals[i][j];
            wl_stats->num_patient_deaths[i][j] = wl_stats->sum_patient_deaths[i][j] = p_lost.number_dead[i][j];
            wl_stats->num_patient_reneges[i][j] = wl_stats->sum_patient_reneges[i][j] = p_lost.number_renege[i][j];
            wl_stats->num_patients_served[i][j] = wl_stats->sum_patient_served[i][j] = waiting_list.num_completions[i][j];

            // Transplant center
            t_stats->num_arrivals[i][j] = t_stats->sum_arrivals[i][j] = transplant_c.num_arrivals[i][j];
            t_stats->num_rejected[i][j] = t_stats->sum_rejected[i][j] = transplant_c.num_rejections[i][j];
            t_stats->num_transplanted[i][j] = t_stats->sum_transplanted[i][j] = transplant_c.num_completions[i][j];

            t_stats->rejection_perc[i][j] =
                    (t_stats->num_rejected[i][j] == 0) ? 0 :
                    100 * (t_stats->num_rejected[i][j] /
                           (t_stats->num_transplanted[i][j] + t_stats->num_rejected[i][j]));
        }

        // Activation center
        a_stats->num_arrivals[i] = a_stats->sum_arrivals[i] = activation.num_arrivals[i];
        a_stats->num_deaths[i] = a_stats->sum_deaths[i] = p_lost.number_dead[i][none];
        a_stats->num_reneges[i] = a_stats->sum_reneges[i] = p_lost.number_renege[i][none];
        a_stats->num_activated[i] = a_stats->sum_activated[i] = activation.num_completions[i];
    }
}

static void gatherMidResults(stats *statistics, stats *prev_statistics, event_list *events) {
    patient_waiting_list waiting_list = events->patient_arrival;
    organ_bank bank = events->organ_arrival;
    transplant_center transplant_c = events->transplant_arrival;
    activation_center activation = events->activation_arrival;
    patients_lost p_lost = events->patients_loss;
    organs_expired o_exp = events->organs_loss;

    waiting_list_stats *wl_stats = statistics->wl_stats, *p_wl_stats = prev_statistics->wl_stats;
    organ_bank_stats *ob_stats = statistics->ob_stats, *p_ob_stats = prev_statistics->ob_stats;
    transplant_stats *t_stats = statistics->trans_stats, *p_t_stats = prev_statistics->trans_stats;
    activation_stats *a_stats = statistics->act_stats, *p_a_stats = prev_statistics->act_stats;

    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // organ bank
        ob_stats->num_organs_in_queue[i] = bank.queues[i]->number;
        ob_stats->num_organ_outdatings[i] = o_exp.num_renege[i] - p_ob_stats->sum_organ_outdatings[i];
        ob_stats->sum_organ_outdatings[i] = o_exp.num_renege[i];
        for (j = 0; j < NUM_DONOR_TYPES; ++j) {
            ob_stats->num_organ_arrivals[i][j]      = bank.num_arrivals[i][j] - p_ob_stats->sum_organ_arrivals[i][j];
            ob_stats->num_organs_completions[i][j]  = bank.num_completions[i][j] - p_ob_stats->sum_organs_completions[i][j];
            ob_stats->sum_organ_arrivals[i][j]      = bank.num_arrivals[i][j];
            ob_stats->sum_organs_completions[i][j]  = bank.num_completions[i][j];
        }

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            // waiting list
            wl_stats->num_patients_in_queue[i][j]   = waiting_list.blood_type_queues[i]->priority_queue[j]->number;
            wl_stats->num_patient_arrivals[i][j]    = waiting_list.num_arrivals[i][j] - p_wl_stats->sum_patient_arrivals[i][j];
            wl_stats->num_patient_deaths[i][j]      = p_lost.number_dead[i][j] - p_wl_stats->sum_patient_deaths[i][j];
            wl_stats->num_patient_reneges[i][j]     = p_lost.number_renege[i][j] - p_wl_stats->sum_patient_reneges[i][j];
            wl_stats->num_patients_served[i][j]     = waiting_list.num_completions[i][j] - p_wl_stats->sum_patient_served[i][j];
            wl_stats->sum_patient_arrivals[i][j]    = waiting_list.num_arrivals[i][j];
            wl_stats->sum_patient_deaths[i][j]      = p_lost.number_dead[i][j];
            wl_stats->sum_patient_reneges[i][j]     = p_lost.number_renege[i][j];
            wl_stats->sum_patient_served[i][j]      = waiting_list.num_completions[i][j];

            // Transplant center
            t_stats->num_arrivals[i][j]     = transplant_c.num_arrivals[i][j] - p_t_stats->sum_arrivals[i][j];
            t_stats->num_rejected[i][j]     = transplant_c.num_rejections[i][j] - p_t_stats->sum_rejected[i][j];
            t_stats->num_transplanted[i][j] = transplant_c.num_completions[i][j] - p_t_stats->sum_transplanted[i][j];
            t_stats->sum_arrivals[i][j]     = transplant_c.num_arrivals[i][j];
            t_stats->sum_rejected[i][j]     = transplant_c.num_rejections[i][j];
            t_stats->sum_transplanted[i][j] = transplant_c.num_completions[i][j];

            t_stats->rejection_perc[i][j] =
                    (t_stats->num_rejected[i][j] == 0) ? 0 :
                    100 * (t_stats->num_rejected[i][j] /
                           (t_stats->num_transplanted[i][j] + t_stats->num_rejected[i][j]));
        }

        // Activation center
        a_stats->num_arrivals[i]    = activation.num_arrivals[i] - p_a_stats->sum_arrivals[i];
        a_stats->num_deaths[i]      = p_lost.number_dead[i][none] - p_a_stats->sum_deaths[i];
        a_stats->num_reneges[i]     = p_lost.number_renege[i][none] - p_a_stats->sum_reneges[i];
        a_stats->num_activated[i]   = activation.num_completions[i] - p_a_stats->sum_activated[i];
        a_stats->sum_arrivals[i]    = activation.num_arrivals[i];
        a_stats->sum_deaths[i]      = p_lost.number_dead[i][none];
        a_stats->sum_reneges[i]     = p_lost.number_renege[i][none];
        a_stats->sum_activated[i]   = activation.num_completions[i];
    }
}

void gatherResults(stats *statistics, stats *prev_statistics, event_list *events, int iter_num) {
#ifdef FINITE
    gatherFirstResults(statistics, events);
#else
    if (iter_num) {
        gatherMidResults(statistics, prev_statistics, events);
    } else {
        gatherFirstResults(statistics, events);
    }
#endif
}

/**
 * Computes the std deviation for each statistic with confidence intervals and retrieves the population values from
 * the last iteration
 * */
void computeStatistics(stats *statistics, int num_stats) {

    int i,j;
    double sum, u = 1.0 - 0.5 * (1.0 - LOC), t, w;

    organ_bank_stats *ob_stats = statistics->ob_stats;
    waiting_list_stats *wl_stats = statistics->wl_stats;
    transplant_stats *t_stats = statistics->trans_stats;
    activation_stats *a_stats = statistics->act_stats;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // organ bank
        for (j = 0; j < NUM_DONOR_TYPES; ++j) {
            STDEV(sum, ob_stats->std_arrivals[i][j], num_stats)
            CONFIDENCE(u, t, w, ob_stats->std_arrivals[i][j], num_stats)

            STDEV(sum, ob_stats->std_completions[i][j], num_stats)
            CONFIDENCE(u, t, w, ob_stats->std_completions[i][j], num_stats)

            STDEV(sum, ob_stats->std_service[i][j], num_stats)
            CONFIDENCE(u, t, w, ob_stats->std_service[i][j], num_stats)

            STDEV(sum, ob_stats->std_wait[i][j], num_stats)
            CONFIDENCE(u, t, w, ob_stats->std_wait[i][j], num_stats)

            STDEV(sum, ob_stats->std_delay[i][j], num_stats)
            CONFIDENCE(u, t, w, ob_stats->std_delay[i][j], num_stats)
        }

        STDEV(sum, ob_stats->std_outdatings[i], num_stats)
        CONFIDENCE(u, t, w, ob_stats->std_outdatings[i], num_stats)

        STDEV(sum, ob_stats->std_interarrival_time[i], num_stats)
        CONFIDENCE(u, t, w, ob_stats->std_interarrival_time[i], num_stats)

        STDEV(sum, ob_stats->std_in_queue[i], num_stats)
        CONFIDENCE(u, t, w, ob_stats->std_in_queue[i], num_stats)

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            // waiting list
            STDEV(sum, wl_stats->std_arrivals[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_arrivals[i][j], num_stats)

            STDEV(sum, wl_stats->std_deaths[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_deaths[i][j], num_stats)

            STDEV(sum, wl_stats->std_reneges[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_reneges[i][j], num_stats)

            STDEV(sum, wl_stats->std_interarrival_time[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_interarrival_time[i][j], num_stats)

            STDEV(sum, wl_stats->std_wait[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_wait[i][j], num_stats)

            STDEV(sum, wl_stats->std_delay[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_delay[i][j], num_stats)

            STDEV(sum, wl_stats->std_service[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_service[i][j], num_stats)

            STDEV(sum, wl_stats->std_in_node[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_in_node[i][j], num_stats)

            STDEV(sum, wl_stats->std_in_queue[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->std_in_queue[i][j], num_stats)

            STDEV(sum, wl_stats->utilization[i][j], num_stats)
            CONFIDENCE(u, t, w, wl_stats->utilization[i][j], num_stats)

            // Transplant center
            STDEV(sum, t_stats->std_arrivals[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_arrivals[i][j], num_stats)

            STDEV(sum, t_stats->std_transplanted[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_transplanted[i][j], num_stats)

            STDEV(sum, t_stats->std_rejected[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_rejected[i][j], num_stats)

            STDEV(sum, t_stats->std_wait[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_wait[i][j], num_stats)

            STDEV(sum, t_stats->std_delay[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_delay[i][j], num_stats)

            STDEV(sum, t_stats->std_service[i][j], num_stats)
            CONFIDENCE(u, t, w, t_stats->std_service[i][j], num_stats)

            STDEV(sum, t_stats->std_in_node, num_stats) // fixme
            CONFIDENCE(u, t, w, t_stats->std_in_node, num_stats) // fixme
        }

        // Activation center
        STDEV(sum, a_stats->std_in_node[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_in_node[i], num_stats)

        STDEV(sum, a_stats->std_delay[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_delay[i], num_stats)

        STDEV(sum, a_stats->std_wait[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_wait[i], num_stats)

        STDEV(sum, a_stats->std_service[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_service[i], num_stats)

        STDEV(sum, a_stats->std_arrivals[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_arrivals[i], num_stats)

        STDEV(sum, a_stats->std_deaths[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_deaths[i], num_stats)

        STDEV(sum, a_stats->std_reneges[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_reneges[i], num_stats)

        STDEV(sum, a_stats->std_activated[i], num_stats)
        CONFIDENCE(u, t, w, a_stats->std_activated[i], num_stats)
    }
}

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats) {
    double number_inactive = events->activation_arrival.total_number;
    double number_trans = events->transplant_arrival.total_number;
    double number_wl, number_bank;
    double diff = t->next - t->current;

    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {

        /* Update organ bank integrals */
        number_bank = events->organ_arrival.queues[i]->number;
        if (number_bank > 0) {
            ti_stats->area_bank[i]->node += diff * (number_bank);
            ti_stats->area_bank[i]->queue += diff * (number_bank-1);
            ti_stats->area_bank[i]->service += diff;
        }

        /* Update waiting list integrals */
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            number_wl = events->patient_arrival.blood_type_queues[i]->priority_queue[j]->number;
            if (number_wl > 0) {
                ti_stats->area_waiting_list[i][j]->node += diff * (number_wl);
                ti_stats->area_waiting_list[i][j]->queue += diff * (number_wl-1);
                ti_stats->area_waiting_list[i][j]->service += diff;
            }
        }
    }

    /* Update activation_center center integrals */
    if (number_inactive > 0) {
        ti_stats->area_activation->node += diff * number_inactive;
        ti_stats->area_activation->queue += diff * (number_inactive-number_inactive);
        ti_stats->area_activation->service += diff * number_inactive;
    }

    /* Update transplant center integrals */
    if (number_trans > 0) {
        ti_stats->area_transplant->node += diff * number_trans;
        ti_stats->area_transplant->queue += diff * (number_trans-number_trans);
        ti_stats->area_transplant->service += diff * number_trans;
    }
}

/**
 * Welford's one-pass algorithm to compute the mean and the standard deviation for each statistic
 * */
void welford(int iter, stats *stat, stats *batch){
    int i,j;
    double diff;

    organ_bank_stats *ob_batch = batch->ob_stats, *ob_stat = stat->ob_stats;
    waiting_list_stats *wl_batch = batch->wl_stats, *wl_stat = stat->wl_stats;
    transplant_stats *t_batch = batch->trans_stats, *t_stat = stat->trans_stats;
    activation_stats *a_batch = batch->act_stats, *a_stat = stat->act_stats;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        for (j = 0; j < NUM_DONOR_TYPES; ++j) {
            WELFORD(diff, ob_batch->num_organ_arrivals[i][j], ob_stat->avg_arrivals[i][j], ob_stat->std_arrivals[i][j], iter)
            WELFORD(diff, ob_batch->num_organs_completions[i][j], ob_stat->avg_completions[i][j], ob_stat->std_completions[i][j], iter)
            WELFORD(diff, ob_batch->avg_wait[i][j], ob_stat->avg_wait[i][j], ob_stat->std_wait[i][j], iter)
            WELFORD(diff, ob_batch->avg_delay[i][j], ob_stat->avg_delay[i][j], ob_stat->std_delay[i][j], iter)
            WELFORD(diff, ob_batch->avg_service[i][j], ob_stat->avg_service[i][j], ob_stat->std_service[i][j], iter)
        }
        WELFORD(diff, ob_batch->num_organ_outdatings[i], ob_stat->avg_outdatings[i], ob_stat->std_outdatings[i], iter)
        WELFORD(diff, ob_batch->avg_in_queue[i],ob_stat->avg_in_queue[i],ob_stat->std_in_queue[i],iter)
        WELFORD(diff, ob_batch->avg_interarrival_time[i],ob_stat->avg_interarrival_time[i],ob_stat->std_interarrival_time[i],iter)

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            // Waiting list
            WELFORD(diff, wl_batch->num_patient_arrivals[i][j], wl_stat->avg_arrivals[i][j], wl_stat->std_arrivals[i][j], iter)
            WELFORD(diff, wl_batch->num_patient_deaths[i][j], wl_stat->avg_deaths[i][j], wl_stat->std_deaths[i][j], iter)
            WELFORD(diff, wl_batch->num_patient_reneges[i][j], wl_stat->avg_reneges[i][j], wl_stat->std_reneges[i][j], iter)
            WELFORD(diff, wl_batch->num_patients_served[i][j], wl_stat->avg_served[i][j], wl_stat->std_served[i][j], iter)
            WELFORD(diff, wl_batch->avg_interarrival_time[i][j], wl_stat->avg_interarrival_time[i][j],wl_stat->std_interarrival_time[i][j], iter)
            WELFORD(diff, wl_batch->avg_wait[i][j],wl_stat->avg_wait[i][j],wl_stat->std_wait[i][j],iter)
            WELFORD(diff, wl_batch->avg_delay[i][j],wl_stat->avg_delay[i][j],wl_stat->std_delay[i][j],iter)
            WELFORD(diff, wl_batch->avg_service[i][j],wl_stat->avg_service[i][j],wl_stat->std_service[i][j],iter)
            WELFORD(diff, wl_batch->avg_in_node[i][j],wl_stat->avg_in_node[i][j],wl_stat->std_in_node[i][j],iter)
            WELFORD(diff, wl_batch->avg_in_queue[i][j],wl_stat->avg_in_queue[i][j],wl_stat->std_in_queue[i][j],iter)
            WELFORD(diff, wl_batch->utilization[i][j],wl_stat->utilization[i][j],wl_stat->std_utilization[i][j],iter)

            // Transplant center
            WELFORD(diff, t_batch->num_arrivals[i][j], t_stat->avg_arrivals[i][j], t_stat->std_arrivals[i][j], iter)
            WELFORD(diff, t_batch->num_transplanted[i][j], t_stat->avg_transplanted[i][j], t_stat->std_transplanted[i][j], iter)
            WELFORD(diff, t_batch->num_rejected[i][j], t_stat->avg_rejected[i][j], t_stat->std_rejected[i][j], iter)
            WELFORD(diff, t_batch->avg_wait[i][j], t_stat->avg_wait[i][j], t_stat->std_wait[i][j],iter)
            WELFORD(diff, t_batch->avg_service[i][j], t_stat->avg_service[i][j], t_stat->std_service[i][j],iter)
            WELFORD(diff, t_batch->avg_delay[i][j], t_stat->avg_delay[i][j], t_stat->std_delay[i][j],iter)
        }

        // Activation center
        WELFORD(diff, a_batch->num_arrivals[i],a_stat->avg_arrivals[i],a_stat->std_arrivals[i],iter)
        WELFORD(diff, a_batch->num_deaths[i],a_stat->avg_deaths[i],a_stat->std_deaths[i],iter)
        WELFORD(diff, a_batch->num_reneges[i],a_stat->avg_reneges[i],a_stat->std_reneges[i],iter)
        WELFORD(diff, a_batch->num_activated[i],a_stat->avg_activated[i],a_stat->std_activated[i],iter)
        WELFORD(diff, a_batch->avg_in_node[i],a_stat->avg_in_node[i],a_stat->std_in_node[i],iter)
        WELFORD(diff, a_batch->avg_wait[i], a_stat->avg_wait[i], a_stat->std_wait[i], iter)
        WELFORD(diff, a_batch->avg_service[i], a_stat->avg_service[i], a_stat->std_service[i], iter)
        WELFORD(diff, a_batch->avg_delay[i],a_stat->avg_delay[i],a_stat->std_delay[i],iter)
    }

    WELFORD(diff, t_batch->avg_in_node,t_stat->avg_in_node,t_stat->std_in_node,iter)
}