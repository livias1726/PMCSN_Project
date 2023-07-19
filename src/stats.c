#include "headers/utils.h"

#define PRINT_TI_STATS(area, time) \
    printf("\t\taverage # in the node...%f\n", area->node/time); \
    printf("\t\taverage # in the queue...%f\n", area->queue/time); \
    printf("\t\tutilization...%f\n", area->service/time);

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

void computeTimeAveragedStats2(stats *stats, time_integrated_stats *ti_stats) {
    double curr = ti_stats->current_time;
    double population;
    area * curr_area;

    waiting_list_stats* wl_stats = stats->wl_stats;
    organ_bank_stats* ob_stats = stats->ob_stats;
    transplant_stats* trans_stats = stats->trans_stats;
    activation_stats* act_stats = stats->act_stats;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        // Organ bank
        curr_area = ti_stats->area_bank[i];
        population = ob_stats->num_organ_arrivals[i];

        ob_stats->avg_interarrival_time[i] = curr / population; //FIXME: use t.last
        ob_stats->avg_wait[i] = curr_area->node / population;
        ob_stats->avg_delay[i] = curr_area->queue / population;
        ob_stats->avg_service[i] = curr_area->service / population;
        ob_stats->avg_in_node[i] = curr_area->node / curr;
        ob_stats->avg_in_queue[i] = curr_area->queue / curr;

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            // Waiting list
            curr_area = ti_stats->area_waiting_list[i][j];
            population = wl_stats->num_patient_arrivals[i][j];

            wl_stats->avg_interarrival_time[i][j] = curr / population; //FIXME: use t.last
            wl_stats->avg_wait[i][j] = curr_area->node / population;
            wl_stats->avg_delay[i][j] = curr_area->queue / population;
            wl_stats->avg_service[i][j] = curr_area->service / population;
            wl_stats->avg_in_node[i][j] = curr_area->node / curr;
            wl_stats->avg_in_queue[i][j] = curr_area->queue / curr;
            wl_stats->utilization[i][j] = curr_area->service / curr;

            /*
            wl_stats->avg_interarrival_time[i][0] += curr / population; //FIXME: use t.last
            wl_stats->avg_wait[i][0] += curr_area->node / population;
            wl_stats->avg_delay[i][0] += curr_area->queue / population;
            wl_stats->avg_service[i][0] += curr_area->service / population;
            wl_stats->avg_in_node[i][0] += curr_area->node / curr;
            wl_stats->avg_in_queue[i][0] += curr_area->queue / curr;
            wl_stats->utilization[i][0] += curr_area->service / curr;
             */
        }
    }

    // Activation center
    curr_area = ti_stats->area_activation;
    act_stats->avg_in_node = curr_area->node / curr;

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
    transplant transplant_c = events->transplant_arrival;
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
        }
    }

    statistics->trans_stats->num_transplants[success] = transplant_c.completed_transplants;
    statistics->trans_stats->num_transplants[reject] = transplant_c.rejected_transplants;
}
