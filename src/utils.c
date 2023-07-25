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
             "Avg inter-arrival times,CI inter-arrival times,Avg wait,CI wait,Avg delay,CI delay,Avg service time,"
             "CI service time,Avg # in the node,CI # in the node,Avg # in the queue,CI # in the queue,Utilization,"
             "CI utilization\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f,"
                       "%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f,%f,+/-%f\n",
                       bt_to_str[i], pr_to_str[j],
                    wl_stats->num_patient_arrivals[i][j], wl_stats->num_patient_deaths[i][j],
                    wl_stats->num_patient_reneges[i][j], wl_stats->num_patients_in_queue[i][j],
                    wl_stats->avg_interarrival_time[i][j], wl_stats->std_interarrival_time[i][j],
                    wl_stats->avg_wait[i][j], wl_stats->std_wait[i][j],
                    wl_stats->avg_delay[i][j], wl_stats->std_delay[i][j],
                    wl_stats->avg_service[i][j], wl_stats->std_service[i][j],
                    wl_stats->avg_in_node[i][j], wl_stats->std_in_node[i][j],
                    wl_stats->avg_in_queue[i][j], wl_stats->std_in_queue[i][j],
                    wl_stats->utilization[i][j], wl_stats->std_utilization[i][j]);
        }
    }
    fclose(f);

    // Organs
    snprintf(path, MAX_LEN, "output/organs_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    organ_bank_stats *ob_stats = statistics->ob_stats;
    header = "Blood type,Organs arrived,Organs outdated,Organs in queue,"
             "Avg inter-arrival times,CI inter-arrival times,Avg # in the queue,CI # in the queue\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f, "%s,%f,%f,%f,"
                   "%f,+/-%f,%f,+/-%f\n",
                bt_to_str[i],
                ob_stats->num_organ_arrivals[i], ob_stats->num_organ_outdatings[i], ob_stats->num_organs_in_queue[i],
                ob_stats->avg_interarrival_time[i], ob_stats->std_interarrival_time[i],
                ob_stats->avg_in_queue[i], ob_stats->std_in_queue[i]);
    }
    fclose(f);

    // Activation center
    snprintf(path, MAX_LEN, "output/activation_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    header = "Blood type,Patients activated,Avg delay,CI delay,Avg # in the node,CI # in the node\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f, "%s,%f,%f,+/-%f,%f,+/-%f\n", bt_to_str[i],
                statistics->act_stats->num_activated[i],
                statistics->act_stats->avg_delay, statistics->act_stats->std_delay,
                statistics->act_stats->avg_in_node, statistics->act_stats->std_in_node);
    }
    fclose(f);

    // Transplant
    snprintf(path, MAX_LEN, "output/transplant_%s_%s.csv", model, policy);
    OPEN_FILE(f, path)
    transplant_stats *trans_stats = statistics->trans_stats;
    header = "Blood type,Priority,Successful transplants,Rejected transplants,Rejection percentage,"
             "Avg # in the node,CI # in the node\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f,+/-%f\n", bt_to_str[i], pr_to_str[j],
                    trans_stats->completed_transplants[i][j], trans_stats->rejected_transplants[i][j],
                    trans_stats->rejection_perc[i][j], trans_stats->avg_in_node, trans_stats->std_in_node);
        }
    }
    fclose(f);
}

void cleanUpOrganBank(organ_bank *bank) {
    organ *current; // head
    organ *next; // first node

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        current = bank->queues[i]->queue; // head
        next = current->next; // first node

        while(next != NULL){
            current = next;
            next = next->next;

            free(current);
        }

        free(bank->queues[i]->queue);
        free(bank->queues[i]);
    }
}

void cleanUpWaitingList(patient_waiting_list *wt_list) {
    patient *current; // head
    patient *next; // first node
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            current = wt_list->blood_type_queues[i]->priority_queue[j]->queue;
            next = current->next;

            while(next != NULL){
                current = next;
                next = next->next;

                free(current);
            }

            free(wt_list->blood_type_queues[i]->priority_queue[j]->queue);
            free(wt_list->blood_type_queues[i]->priority_queue[j]);
        }
        free(wt_list->blood_type_queues[i]);
    }
}

void cleanUpActivationCenter(activation_center *act_center) {
    in_activation *current = act_center->inactive_patients; // head
    in_activation *next = current->next; // first node

    while(next != NULL){
        current = next;
        next = next->next;

        free(current->patient);
        free(current);
    }

    free(act_center->inactive_patients);
}

void cleanUpTransplantCenter(transplant_center *trans_center) {
    in_transplant *current = trans_center->transplanted_patients; // head
    in_transplant *next = current->next; // first node

    while(next != NULL){
        current = next;
        next = next->next;

        free(current->matched);
        free(current);
    }

    free(trans_center->transplanted_patients); // free head
}

void cleanUpEventList(event_list* events){

    cleanUpOrganBank(&events->organ_arrival);
    cleanUpWaitingList(&events->patient_arrival);
    cleanUpActivationCenter(&events->activation_arrival);
    cleanUpTransplantCenter(&events->transplant_arrival);

    free(events);
}

void cleanUpTimeStatistics(time_integrated_stats *ti_stats) {
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(ti_stats->area_bank[i]);

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            free(ti_stats->area_waiting_list[i][j]);
        }
    }

    free(ti_stats->area_transplant);
    free(ti_stats->area_activation);

    free(ti_stats);
}

void cleanUpStatistics(stats *statistics){

    free(statistics->wl_stats);
    free(statistics->ob_stats);
    free(statistics->act_stats);
    free(statistics->trans_stats);

    free(statistics);
}

void shuffle(BLOOD_TYPE *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            BLOOD_TYPE t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}