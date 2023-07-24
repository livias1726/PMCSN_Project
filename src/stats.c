#include "headers/utils.h"

/**
 * This file contains the implementation for every procedure that handles statistics.
 *
 * The statistics managed by the simulations are divided by center, according to its functions:
 *      - WAITING LISTS
 *          Avg inter-arrival times,
 *          Avg wait,
 *          Avg delay,
 *          Avg service time,
 *          Avg # in the node,
 *          Avg # in the queue,
 *          Utilization
 *      - ORGAN BANK
 *          Avg inter-arrival times,
 *          Avg # in the queue
 *      - ACTIVATION
 *          Avg delay,
 *          Avg # in the node
 *      - TRANSPLANT
 *          Rejection percentage,
 *          Avg # in the node
 * */

#define PRINT_TI_STATS(area, time) \
    printf("\t\taverage # in the node...%f\n", area->node/time); \
    printf("\t\taverage # in the queue...%f\n", area->queue/time); \
    printf("\t\tutilization...%f\n", area->service/time);
/*
void computeTimeAveragedStats(time_integrated_stats *stats) {
    double curr = stats->current_time;
    area *curr_area;
    int i,j;

    // Waiting list
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            curr_area = stats->area_waiting_list[i][j];
            printf("Waiting list - BLOOD TYPE %s - PRIORITY %s: \n", bt_to_str[i], pr_to_str[j]);
            PRINT_TI_STATS(curr_area, curr)
        }
    }

    // Organ bank
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        curr_area = stats->area_bank[i];
        printf("Organ bank - BLOOD TYPE %s: \n", bt_to_str[i]);
        PRINT_TI_STATS(curr_area, curr)
    }

    // Activation center
    curr_area = stats->area_activation;
    printf("Activation center: \n");
    PRINT_TI_STATS(curr_area, curr)

    // Transplant center
    curr_area = stats->area_transplant;
    printf("Transplant center: \n");
    PRINT_TI_STATS(curr_area, curr)
}
 */

void computeTimeAveragedStats2(stats *stats, time_integrated_stats *ti_stats, sim_time *t) {
    double curr = t->current;
    double population, completion;
    area * curr_area;

    waiting_list_stats* wl_stats = stats->wl_stats;
    organ_bank_stats* ob_stats = stats->ob_stats;
    transplant_stats* trans_stats = stats->trans_stats;
    activation_stats* act_stats = stats->act_stats;

    // Activation center
    curr_area = ti_stats->area_activation;
    completion = act_stats->num_activated;
    act_stats->avg_in_node = curr_area->node / curr;
    act_stats->avg_delay = curr_area->node / completion;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        curr_area = ti_stats->area_bank[i];
        population = ob_stats->num_organ_arrivals[i];

        ob_stats->avg_interarrival_time[i] = t->last[organ_arrival] / population;
        ob_stats->avg_in_queue[i] = curr_area->queue / curr;

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            // FIXME: il tempo di attesa dei low deve essere sommato al tempo di attesa in attivazione!!!
            // Waiting list
            curr_area = ti_stats->area_waiting_list[i][j];
            population = wl_stats->num_patient_arrivals[i][j];
            completion = wl_stats->num_patients_served[i][j];

            wl_stats->avg_interarrival_time[i][j] = t->last[patient_arrival] / population;
            wl_stats->avg_wait[i][j] = curr_area->node / completion;
            wl_stats->avg_delay[i][j] = curr_area->queue / completion;
            wl_stats->avg_service[i][j] = curr_area->service / completion;
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
void gatherResults(stats* statistics, event_list *events){
    patient_waiting_list waiting_list = events->patient_arrival;
    organ_bank bank = events->organ_arrival;
    transplant_center transplant_c = events->transplant_arrival;
    activation_center activation = events->activation_arrival;
    organs_expired organs_expired = events->organs_loss;
    patients_lost patients_lost = events->patients_loss;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        statistics->ob_stats->num_organs_in_queue[i] = bank.queues[i]->number;
        statistics->ob_stats->num_organ_arrivals[i] = bank.num_arrivals[i];
        statistics->ob_stats->num_organ_outdatings[i] = organs_expired.number[i];

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            statistics->wl_stats->num_patients_in_queue[i][j] = waiting_list.blood_type_queues[i]->priority_queue[j]->number;
            statistics->wl_stats->num_patient_arrivals[i][j] = waiting_list.num_arrivals[i][j];
            statistics->wl_stats->num_patient_deaths[i][j] = patients_lost.number_dead[i][j];
            statistics->wl_stats->num_patient_reneges[i][j] = patients_lost.number_renege[i][j];
            statistics->wl_stats->num_patients_served[i][j] = waiting_list.num_completions[i][j];

            statistics->trans_stats->completed_transplants[i][j] = transplant_c.completed_transplants[i][j];
            statistics->trans_stats->rejected_transplants[i][j] = transplant_c.rejected_transplants[i][j];
        }
    }

    statistics->act_stats->num_activated = activation.activated_number;
}

/**
 * Computes the std deviation for each statistic with confidence intervals and retrieves the population values from
 * the last iteration
 * */
void computeFinalStatistics(stats *final_stat, stats **statistics, int num_stats) {

    int i,j,k;
    double sum;
    double u = 1.0 - 0.5 * (1.0 - LOC), t, w;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {

        final_stat->ob_stats->num_organs_in_queue[i] = statistics[num_stats-1]->ob_stats->num_organs_in_queue[i];
        final_stat->ob_stats->num_organ_arrivals[i] = statistics[num_stats-1]->ob_stats->num_organ_arrivals[i];
        final_stat->ob_stats->num_organ_outdatings[i] = statistics[num_stats-1]->ob_stats->num_organ_outdatings[i];

        STDEV(sum, final_stat->ob_stats->std_interarrival_time[i], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_interarrival_time[i],num_stats)

        STDEV(sum, final_stat->ob_stats->std_in_queue[i], num_stats)
        CONFIDENCE(u,t,w,final_stat->ob_stats->std_in_queue[i],num_stats)

        for (j = 0; j < NUM_PRIORITIES; ++j) {

            final_stat->wl_stats->num_patients_in_queue[i][j] = statistics[num_stats-1]->wl_stats->num_patients_in_queue[i][j];
            final_stat->wl_stats->num_patient_arrivals[i][j] = statistics[num_stats-1]->wl_stats->num_patient_arrivals[i][j];
            final_stat->wl_stats->num_patient_deaths[i][j] = statistics[num_stats-1]->wl_stats->num_patient_deaths[i][j];
            final_stat->wl_stats->num_patient_reneges[i][j] = statistics[num_stats-1]->wl_stats->num_patient_reneges[i][j];
            final_stat->wl_stats->num_patients_served[i][j] = statistics[num_stats-1]->wl_stats->num_patients_served[i][j];

            final_stat->trans_stats->completed_transplants[i][j] = statistics[num_stats-1]->trans_stats->completed_transplants[i][j];
            final_stat->trans_stats->rejected_transplants[i][j] = statistics[num_stats-1]->trans_stats->rejected_transplants[i][j];

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
