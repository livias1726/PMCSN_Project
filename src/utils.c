#include "headers/utils.h"

void saveResultsCsv(stats* statistics){
    FILE *f;
    char path[MAX_LEN];
    char *model, *policy, *header;
    int i,j;

#ifdef REAL
    model = "real";
#else
    model = "dummy";
#endif

#ifdef ABO_ID
    policy = "id";
#else
    policy = "comp";
#endif

    // Patients
    snprintf(path, MAX_LEN, "output/waiting_list_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    waiting_list_stats *wl_stats = statistics->wl_stats;
    header = "Blood type,Priority,Patients arrived,Patients dead,Patients reneged,Patients in queue,"
             "Avg inter-arrival times,Avg wait,Avg delay,Avg service time,Avg # in the node,Avg # in the queue,"
             "Utilization\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", bt_to_str[i], pr_to_str[j],
                    wl_stats->num_patient_arrivals[i][j], wl_stats->num_patient_deaths[i][j],
                    wl_stats->num_patient_reneges[i][j], wl_stats->num_patients_in_queue[i][j],
                    wl_stats->avg_interarrival_time[i][j], wl_stats->avg_wait[i][j], wl_stats->avg_delay[i][j],
                    wl_stats->avg_service[i][j], wl_stats->avg_in_node[i][j], wl_stats->avg_in_queue[i][j],
                    wl_stats->utilization[i][j]);
        }
    }
    fclose(f);

    // Organs
    snprintf(path, MAX_LEN, "output/organs_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    organ_bank_stats *ob_stats = statistics->ob_stats;
    header = "Blood type,Organs arrived,Organs outdated,Organs in queue,Avg inter-arrival times,Avg wait,Avg delay,"
             "Avg service time,Avg # in the node,Avg # in the queue\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", bt_to_str[i],
                ob_stats->num_organ_arrivals[i], ob_stats->num_organ_outdatings[i], ob_stats->num_organs_in_queue[i],
                ob_stats->avg_interarrival_time[i], ob_stats->avg_wait[i], ob_stats->avg_delay[i],
                ob_stats->avg_service[i], ob_stats->avg_in_node[i], ob_stats->avg_in_queue[i]);
    }
    fclose(f);

    // Activation center
    snprintf(path, MAX_LEN, "output/activation_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    header = "Avg # in the node\n";
    fprintf(f, "%s", header);
    fprintf(f, "%f\n", statistics->act_stats->avg_in_node);
    fclose(f);

    // Transplant
    snprintf(path, MAX_LEN, "output/transplant_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    transplant_stats *trans_stats = statistics->trans_stats;
    header = "Blood type,Priority,Successful transplants,Rejected transplants,Rejection percentage,Avg # in the node\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f\n", bt_to_str[i], pr_to_str[j], trans_stats->completed_transplants[i][j],
                    trans_stats->rejected_transplants[i][j], trans_stats->rejection_perc[i][j], trans_stats->avg_in_node);
        }
    }
    fclose(f);
}

void printResults(stats* statistics, FILE* ch){
    // waiting list
    print_by_all("Patients arrived:\n", statistics->wl_stats->num_patient_arrivals, ch)
    print_by_all("Patients dead:\n", statistics->wl_stats->num_patient_deaths, ch)
    print_by_all("Patients reneged:\n", statistics->wl_stats->num_patient_reneges, ch)
    print_by_all("Patients in queue:\n", statistics->wl_stats->num_patients_in_queue, ch)
    fprintf(stdout, "%s", "\tWaiting list time integrated statistics:\n");
    print_by_all("\tAverage inter-arrival time:\n", statistics->wl_stats->avg_interarrival_time, ch)
    print_by_all("\tAverage wait:\n", statistics->wl_stats->avg_wait, ch)
    print_by_all("\tAverage delay:\n", statistics->wl_stats->avg_delay, ch)
    print_by_all("\tAverage service time:\n", statistics->wl_stats->avg_service, ch)
    print_by_all("\tAverage # in the node:\n", statistics->wl_stats->avg_in_node, ch)
    print_by_all("\tAverage # in the queue:\n", statistics->wl_stats->avg_in_queue, ch)
    print_by_all("\tUtilization:\n", statistics->wl_stats->utilization, ch)

    // organ bank
    print_by_blood_type("Organs arrived:\n", statistics->ob_stats->num_organ_arrivals, ch)
    print_by_blood_type("Organs outdated:\n", statistics->ob_stats->num_organ_outdatings, ch)
    print_by_blood_type("Organs in queue:\n", statistics->ob_stats->num_organs_in_queue, ch)
    fprintf(stdout, "%s", "Organ bank time integrated statistics:\n");
    print_by_blood_type("\tAverage inter-arrival time:\n", statistics->ob_stats->avg_interarrival_time, ch)
    print_by_blood_type("\tAverage wait:\n", statistics->ob_stats->avg_wait, ch)
    print_by_blood_type("\tAverage delay:\n", statistics->ob_stats->avg_delay, ch)
    print_by_blood_type("\tAverage service time:\n", statistics->ob_stats->avg_service, ch)
    print_by_blood_type("\tAverage # in the node:\n", statistics->ob_stats->avg_in_node, ch)
    print_by_blood_type("\tAverage # in the queue:\n", statistics->ob_stats->avg_in_queue, ch)

    // Activation center
    fprintf(ch, "%s", "\tActivation center time integrated statistics:\n");
    fprintf(ch,"\t\tAverage # in the node: %f\n", statistics->act_stats->avg_in_node);

    // Transplant center
    /*
    print_transplants_res("\tTransplants:\n", statistics->trans_stats->completed_transplants, ch)
    fprintf(ch, "%s", "\tTransplant center time integrated statistics:\n");
    fprintf(ch,"\t\tAverage # in the node: %f\n", statistics->trans_stats->avg_in_node);
     */
}

void cleanUpEventList(event_list* events){
    patient_waiting_list *waiting_list = &events->patient_arrival;
    patient_queue_blood_type **bt_queues = waiting_list->blood_type_queues;
    organ_bank *bank = &events->organ_arrival;
    //activation_center *;
    transplant_center *transplants;
    organs_expired organs_loss;
    patients_lost patients_loss;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(bank->queues[i]->queue); //TODO: while there are organs in queue, free organ
        free(bank->queues[i]);

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            free(bt_queues[i]->priority_queue[j]->queue); //TODO: while there are patients in queue, free patient
            free(bt_queues[i]->priority_queue[j]);
        }

        free(bt_queues[i]);
    }

    transplant_center *trans_queue = &events->transplant_arrival;
    free(trans_queue->transplanted_patients); //TODO: while there are patients in queue, free patient

    activation_center *inactive_queue = &events->activation_arrival;
    free(inactive_queue->inactive_patients->patient); //TODO: while there are patients in queue, free patient
    free(inactive_queue->inactive_patients);

    //TODO: free the other structured into events
}

void cleanUpTimeStatistics(time_integrated_stats *ti_stats) {

}

void cleanUpStatistics(stats *statistics){

}