#include "headers/utils.h"

void saveResultsCsv(stats* statistics){
    FILE *f;
    char path[MAX_LEN];
    char *model, *policy;
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

    snprintf(path, MAX_LEN, "output/res_%s_%s.csv", model, policy);
    if((f = fopen(path, "w")) == NULL) {
        fprintf(stderr, "Cannot open output file");
        exit(EXIT_FAILURE);
    }

    int i,j;

    // Patients
    char *header = "Blood type,Priority,Patients arrived,Patients dead,Patients reneged,Patients in queue,"
                   "WL Avg inter-arrival times,WL Avg wait,WL Avg delay,WL Avg service time,"
                   "WL Avg # in the node,WL Avg # in the queue,WL utilization\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", bt_to_str[i], pr_to_str[j],
                    statistics->wl_stats->num_patient_arrivals[i][j], statistics->wl_stats->num_patient_deaths[i][j],
                    statistics->wl_stats->num_patient_reneges[i][j],statistics->wl_stats->num_patients_in_queue[i][j],
                    statistics->wl_stats->avg_interarrival_time[i][j], statistics->wl_stats->avg_wait[i][j],
                    statistics->wl_stats->avg_delay[i][j], statistics->wl_stats->avg_service[i][j],
                    statistics->wl_stats->avg_in_node[i][j], statistics->wl_stats->avg_in_queue[i][j],
                    statistics->wl_stats->utilization[i][j]);
        }
    }
    fprintf(f, "%s", "\n");

    // Organs
    header = "Blood type,Organs arrived,Organs outdated,Organs in queue,"
             "OB Avg inter-arrival times,OB Avg wait,OB Avg delay,OB Avg service time,"
             "OB Avg # in the node,OB Avg # in the queue\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", bt_to_str[i],
                statistics->ob_stats->num_organ_arrivals[i], statistics->ob_stats->num_organ_outdatings[i],
                statistics->ob_stats->num_organs_in_queue[i], statistics->ob_stats->avg_interarrival_time[i],
                statistics->ob_stats->avg_wait[i], statistics->ob_stats->avg_delay[i],
                statistics->ob_stats->avg_service[i], statistics->ob_stats->avg_in_node[i],
                statistics->ob_stats->avg_in_queue[i]);
    }
    fprintf(f, "%s", "\n");

    // Activation center
    header = "ACT Avg # in the node\n";
    fprintf(f, "%s", header);
    fprintf(f, "%f\n", statistics->act_stats->avg_in_node);

    fprintf(f, "%s", "\n");

    // Transplant
    header = "Successful transplants,Rejected transplants,TX Avg # in the node\n";
    fprintf(f, "%s", header);
    fprintf(f, "%f,%f,%f\n", statistics->trans_stats->num_transplants[success],
            statistics->trans_stats->num_transplants[reject], statistics->trans_stats->avg_in_node);

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
    print_transplants_res("\tTransplants:\n", statistics->trans_stats->num_transplants, ch)
    fprintf(ch, "%s", "\tTransplant center time integrated statistics:\n");
    fprintf(ch,"\t\tAverage # in the node: %f\n", statistics->trans_stats->avg_in_node);
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