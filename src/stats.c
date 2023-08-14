#include "headers/utils.h"

/**
 * This file contains the implementation for every procedure that handles statistics.
 * */

void computeTimeAveragedStats(stats *stats, time_integrated_stats *ti_stats, sim_time *t) {
    double curr = t->current;
    double population_bank, population_wl, completion;
    area * curr_area;

    waiting_list_stats* wl_stats = stats->wl_stats;
    organ_bank_stats* ob_stats = stats->ob_stats;
    transplant_stats* trans_stats = stats->trans_stats;
    activation_stats* act_stats = stats->act_stats;
    int i,j;

    // Activation
    curr_area = ti_stats->area_activation;
    completion = act_stats->num_activated;

    act_stats->avg_in_node = curr_area->node / curr;
    act_stats->avg_delay = (completion == 0) ? 0 : curr_area->node / completion;

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
        population_bank = ob_stats->sum_organ_arrivals[i][0];

        ob_stats->avg_interarrival_time[i] = (population_bank == 0) ? 0 : t->last[organ_arrival] / population_bank;
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
    }

    // Transplant center
    curr_area = ti_stats->area_transplant;
    trans_stats->avg_in_node = curr_area->node / curr;
}

/**
 * Retrieves the data taken during the simulation
 * */
void gatherResults(stats *statistics, stats *prev_statistics, event_list *events, int iter_num) {
    patient_waiting_list waiting_list = events->patient_arrival;
    organ_bank bank = events->organ_arrival;
    transplant_center transplant_c = events->transplant_arrival;
    activation_center activation = events->activation_arrival;
    patients_lost p_lost = events->patients_loss;
    organs_expired o_exp = events->organs_loss;

    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // organ bank
        statistics->ob_stats->num_organs_in_queue[i] = bank.queues[i]->number;

        if (iter_num != 0) {
            statistics->ob_stats->num_organ_arrivals[i][0] = bank.num_arrivals[i][0]-prev_statistics->ob_stats->sum_organ_arrivals[i][0];
            statistics->ob_stats->num_organ_arrivals[i][1] = bank.num_arrivals[i][1]-prev_statistics->ob_stats->sum_organ_arrivals[i][1];
            statistics->ob_stats->num_organ_outdatings[i] = o_exp.num_renege[i]-prev_statistics->ob_stats->sum_organ_outdatings[i];
        } else {
            statistics->ob_stats->num_organ_arrivals[i][0] = bank.num_arrivals[i][0];
            statistics->ob_stats->num_organ_arrivals[i][1] = bank.num_arrivals[i][1];
            statistics->ob_stats->num_organ_outdatings[i] = o_exp.num_renege[i];
        }

        statistics->ob_stats->sum_organ_arrivals[i][0] = bank.num_arrivals[i][0];
        statistics->ob_stats->sum_organ_arrivals[i][1] = bank.num_arrivals[i][1];
        statistics->ob_stats->sum_organ_outdatings[i] = o_exp.num_renege[i];

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            // waiting list
            statistics->wl_stats->num_patients_in_queue[i][j] = waiting_list.blood_type_queues[i]->priority_queue[j]->number;

            if (iter_num != 0) {
                statistics->wl_stats->num_patient_arrivals[i][j] = waiting_list.num_arrivals[i][j]-prev_statistics->wl_stats->sum_patient_arrivals[i][j];
                statistics->wl_stats->num_patient_deaths[i][j] = p_lost.number_dead[i][j]-prev_statistics->wl_stats->sum_patient_deaths[i][j];
                statistics->wl_stats->num_patient_reneges[i][j] = p_lost.number_renege[i][j]-prev_statistics->wl_stats->sum_patient_reneges[i][j];
                statistics->wl_stats->num_patients_served[i][j] = waiting_list.num_completions[i][j]-prev_statistics->wl_stats->sum_patient_served[i][j];

                statistics->trans_stats->completed_transplants[i][j] = transplant_c.num_completions[i][j];
                statistics->trans_stats->rejected_transplants[i][j] = transplant_c.num_rejections[i][j];
            } else {
                statistics->wl_stats->num_patient_arrivals[i][j] = waiting_list.num_arrivals[i][j];
                statistics->wl_stats->num_patient_deaths[i][j] = p_lost.number_dead[i][j];
                statistics->wl_stats->num_patient_reneges[i][j] = p_lost.number_renege[i][j];
                statistics->wl_stats->num_patients_served[i][j] = waiting_list.num_completions[i][j];

                statistics->trans_stats->completed_transplants[i][j] = transplant_c.num_completions[i][j];
                statistics->trans_stats->rejected_transplants[i][j] = transplant_c.num_rejections[i][j];
            }

            statistics->wl_stats->sum_patient_arrivals[i][j] = waiting_list.num_arrivals[i][j];
            statistics->wl_stats->sum_patient_deaths[i][j] = p_lost.number_dead[i][j];
            statistics->wl_stats->sum_patient_reneges[i][j] = p_lost.number_renege[i][j];
            statistics->wl_stats->sum_patient_served[i][j] = waiting_list.num_completions[i][j];
        }

        // fixme: activation center
        statistics->act_stats->num_deaths += p_lost.number_dead[i][j];
        statistics->act_stats->num_reneges += p_lost.number_renege[i][j];
    }

    // fixme
    statistics->act_stats->num_activated = activation.num_completions;
    statistics->act_stats->num_arrivals = activation.num_arrivals;
}

/**
 * Computes the std deviation for each statistic with confidence intervals and retrieves the population values from
 * the last iteration
 * */
void computeFinalStatistics(stats *final_stat, stats **statistics, int num_stats) {

    int i,j;
    double sum;
    double u = 1.0 - 0.5 * (1.0 - LOC), t, w;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        /*
        final_stat->ob_stats->num_organs_in_queue[i] = statistics[num_stats]->ob_stats->num_organs_in_queue[i];
        final_stat->ob_stats->num_organ_arrivals[i][0] = statistics[num_stats]->ob_stats->num_organ_arrivals[i][0];
        final_stat->ob_stats->num_organ_arrivals[i][1] = statistics[num_stats]->ob_stats->num_organ_arrivals[i][1];
        final_stat->ob_stats->num_organ_outdatings[i] = statistics[num_stats]->ob_stats->num_organ_outdatings[i];
        */
        STDEV(sum, final_stat->ob_stats->std_arrivals[i][0], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_arrivals[i][0], num_stats)

        STDEV(sum, final_stat->ob_stats->std_arrivals[i][1], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_arrivals[i][1], num_stats)

        STDEV(sum, final_stat->ob_stats->std_outdatings[i], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_outdatings[i], num_stats)

        STDEV(sum, final_stat->ob_stats->std_interarrival_time[i], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_interarrival_time[i],num_stats)

        STDEV(sum, final_stat->ob_stats->std_in_queue[i], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_in_queue[i],num_stats)

        for (j = 0; j < NUM_PRIORITIES; ++j) {

            /*final_stat->wl_stats->num_patients_in_queue[i][j] = statistics[num_stats]->wl_stats->num_patients_in_queue[i][j];
            final_stat->wl_stats->num_patient_arrivals[i][j] = statistics[num_stats]->wl_stats->num_patient_arrivals[i][j];
            final_stat->wl_stats->num_patient_deaths[i][j] = statistics[num_stats]->wl_stats->num_patient_deaths[i][j];
            final_stat->wl_stats->num_patient_reneges[i][j] = statistics[num_stats]->wl_stats->num_patient_reneges[i][j];
            final_stat->wl_stats->num_patients_served[i][j] = statistics[num_stats]->wl_stats->num_patients_served[i][j];*/

            STDEV(sum, final_stat->wl_stats->std_arrivals[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_arrivals[i][j], num_stats)

            STDEV(sum, final_stat->wl_stats->std_deaths[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_deaths[i][j], num_stats)

            STDEV(sum, final_stat->wl_stats->std_reneges[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_reneges[i][j], num_stats)

            STDEV(sum, final_stat->wl_stats->std_interarrival_time[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_interarrival_time[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->std_wait[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_wait[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->std_delay[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_delay[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->std_service[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_service[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->std_in_node[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_in_node[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->std_in_queue[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->std_in_queue[i][j],num_stats)

            STDEV(sum, final_stat->wl_stats->utilization[i][j], num_stats)
            CONFIDENCE(u,t,w,final_stat->wl_stats->utilization[i][j],num_stats)

            /*final_stat->trans_stats->completed_transplants[i][j] = statistics[num_stats]->trans_stats->completed_transplants[i][j];
            final_stat->trans_stats->rejected_transplants[i][j] = statistics[num_stats]->trans_stats->rejected_transplants[i][j];*/

            final_stat->trans_stats->rejection_perc[i][j] =
                    (final_stat->trans_stats->rejected_transplants[i][j] == 0) ? 0 :
                    100 * (final_stat->trans_stats->rejected_transplants[i][j] /
                    (final_stat->trans_stats->completed_transplants[i][j]+final_stat->trans_stats->rejected_transplants[i][j]));
        }
    }

    STDEV(sum, final_stat->act_stats->std_in_node, num_stats)
    CONFIDENCE(u,t,w,final_stat->act_stats->std_in_node,num_stats)

    STDEV(sum, final_stat->act_stats->std_delay, num_stats)
    CONFIDENCE(u,t,w,final_stat->act_stats->std_delay,num_stats)

    STDEV(sum, final_stat->trans_stats->std_in_node, num_stats)
    CONFIDENCE(u,t,w,final_stat->trans_stats->std_in_node,num_stats)
}

void updateIntegralsStats(event_list *events, sim_time *t, time_integrated_stats *ti_stats) {
    double number_inactive = events->activation_arrival.total_number;
    double number_trans = events->transplant_arrival.total_number;
    double number_wl, number_bank;
    double diff = t->next - t->current;

    int i,j;

    /* Update waiting list integrals */
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {

        /* Update organ bank integrals */
        number_bank = events->organ_arrival.queues[i]->number;
        //fixme useless: ti_stats->area_bank[i]->node += diff * (number_bank+1);
        ti_stats->area_bank[i]->queue += diff * (number_bank);
        //fixme useless: ti_stats->area_bank[i]->service += diff;

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
    ti_stats->area_activation->node += diff * number_inactive;
    ti_stats->area_activation->service += diff * number_inactive;

    /* Update transplant center integrals */
    ti_stats->area_transplant->node += diff * number_trans;
    ti_stats->area_transplant->service += diff * number_trans;
}

/**
 * Welford's one-pass algorithm to compute the mean and the standard deviation for each statistic
 * */
void welford(int iter, stats *stat, stats *batch){
    double diff;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        WELFORD(diff, batch->ob_stats->num_organ_arrivals[i][0], stat->ob_stats->avg_arrivals[i][0], stat->ob_stats->std_arrivals[i][0], iter);
        WELFORD(diff, batch->ob_stats->num_organ_arrivals[i][1], stat->ob_stats->avg_arrivals[i][1], stat->ob_stats->std_arrivals[i][1], iter);
        WELFORD(diff, batch->ob_stats->num_organ_outdatings[i], stat->ob_stats->avg_outdatings[i], stat->ob_stats->std_outdatings[i], iter);
        WELFORD(diff, batch->ob_stats->avg_interarrival_time[i],stat->ob_stats->avg_interarrival_time[i],stat->ob_stats->std_interarrival_time[i],iter)
        WELFORD(diff, batch->ob_stats->avg_in_queue[i],stat->ob_stats->avg_in_queue[i],stat->ob_stats->std_in_queue[i],iter)

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            // Waiting list
            WELFORD(diff, batch->wl_stats->num_patient_arrivals[i][j], stat->wl_stats->avg_arrivals[i][j], stat->wl_stats->std_arrivals[i][j], iter);
            WELFORD(diff, batch->wl_stats->num_patient_deaths[i][j], stat->wl_stats->avg_deaths[i][j], stat->wl_stats->std_deaths[i][j], iter);
            WELFORD(diff, batch->wl_stats->num_patient_reneges[i][j], stat->wl_stats->avg_reneges[i][j], stat->wl_stats->std_reneges[i][j], iter);
            WELFORD(diff, batch->wl_stats->avg_interarrival_time[i][j], stat->wl_stats->avg_interarrival_time[i][j],stat->wl_stats->std_interarrival_time[i][j], iter)
            WELFORD(diff, batch->wl_stats->avg_wait[i][j],stat->wl_stats->avg_wait[i][j],stat->wl_stats->std_wait[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_delay[i][j],stat->wl_stats->avg_delay[i][j],stat->wl_stats->std_delay[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_service[i][j],stat->wl_stats->avg_service[i][j],stat->wl_stats->std_service[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_in_node[i][j],stat->wl_stats->avg_in_node[i][j],stat->wl_stats->std_in_node[i][j],iter)
            WELFORD(diff, batch->wl_stats->avg_in_queue[i][j],stat->wl_stats->avg_in_queue[i][j],stat->wl_stats->std_in_queue[i][j],iter)
            WELFORD(diff, batch->wl_stats->utilization[i][j],stat->wl_stats->utilization[i][j],stat->wl_stats->std_utilization[i][j],iter)
        }
    }

    // Activation center
    WELFORD(diff, batch->act_stats->num_arrivals,stat->act_stats->avg_arrivals,stat->act_stats->std_arrivals,iter)
    WELFORD(diff, batch->act_stats->num_deaths,stat->act_stats->avg_deaths,stat->act_stats->std_deaths,iter)
    WELFORD(diff, batch->act_stats->num_reneges,stat->act_stats->avg_reneges,stat->act_stats->std_reneges,iter)
    WELFORD(diff, batch->act_stats->num_activated,stat->act_stats->avg_activated,stat->act_stats->std_activated,iter)
    WELFORD(diff, batch->act_stats->avg_in_node,stat->act_stats->avg_in_node,stat->act_stats->std_in_node,iter)
    WELFORD(diff, batch->act_stats->avg_delay,stat->act_stats->avg_delay,stat->act_stats->std_delay,iter)

    // Transplant center
    WELFORD(diff, batch->trans_stats->avg_in_node,stat->trans_stats->avg_in_node,stat->trans_stats->std_in_node,iter)
}