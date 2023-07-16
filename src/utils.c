#include "headers/utils.h"

void saveResultsCsv(stats* statistics){
    FILE *f;
    char path[MAX_LEN];
#ifdef ABO_ID
    snprintf(path, MAX_LEN, "output/res_%s.csv", "id");
#else
    snprintf(path, MAX_LEN, "output/res_%s.csv", "comp");
#endif
    if((f = fopen(path, "w")) == NULL) {
        fprintf(stderr, "Cannot open output file");
        exit(EXIT_FAILURE);
    }

    int i,j;

    // Patients
    char *header = "Blood type,Priority,Patients arrived,Patients dead,Patients reneged,Patients in queue,Average waiting times\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            fprintf(f, "%s,%s,%f,%f,%f,%f,%f\n", bt_to_str[i], pr_to_str[j],
                    statistics->numPatientArrivals[i][j], statistics->numDeaths[i][j], statistics->numReneges[i][j],
                    statistics->numPatients[i][j], statistics->meanGlobalWaitingTimePerQueue[i][j]);
        }
    }
    fprintf(f, "%s", "\n");

    // Organs
    header = "Blood type,Organs arrived,Organs outdated,Organs in queue\n";
    fprintf(f, "%s", header);
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        fprintf(f, "%s,%f,%f,%f\n", bt_to_str[i], statistics->numOrganArrivals[i], statistics->numOrganOutdatings[i],
                statistics->numOrgans[i]);
    }
    fprintf(f, "%s", "\n");

    // Transplant
    fprintf(f, "%s", ",Number of transplant\n");
    fprintf(f, "%s,%f\n", "Successful", statistics->numTransplants[0]);
    fprintf(f, "%s,%f\n", "Rejected", statistics->numTransplants[1]);
    fprintf(f, "%s", "\n");

    fclose(f);
}

void saveResultsTxt(stats* statistics){
    FILE *f;
    char path[MAX_LEN];
#ifdef ABO_ID
    snprintf(path, MAX_LEN, "output/res_%s.txt", "id");
#else
    snprintf(path, MAX_LEN, "output/res_%s.txt", "comp");
#endif
    if((f = fopen(path, "w")) == NULL) {
        fprintf(stderr, "Cannot open output file");
        exit(EXIT_FAILURE);
    }

    print_by_all("Patients arrived:\n", statistics->numPatientArrivals, f)
    print_by_all("Patients dead:\n", statistics->numDeaths, f)
    print_by_all("Patients reneged:\n", statistics->numReneges, f)
    print_by_blood_type("Organs arrived:\n", statistics->numOrganArrivals, f)
    print_by_blood_type("Organs outdated:\n", statistics->numOrganOutdatings, f)
    print_transplants_res("Transplants:\n", statistics->numTransplants, f)
    print_by_blood_type("Organs in queue:\n", statistics->numOrgans, f)
    print_by_all("Patients in queue:\n", statistics->numPatients, f)
    print_by_all("Average waiting times:\n", statistics->meanGlobalWaitingTimePerQueue, f)

    fclose(f);
}

void printResults(stats* statistics){
    printf("\nResults:\n");

    print_by_all("\tPatients arrived:\n", statistics->numPatientArrivals, stdout)
    print_by_all("\tPatients dead:\n", statistics->numDeaths, stdout)
    print_by_all("\tPatients reneged:\n", statistics->numReneges, stdout)
    print_by_blood_type("\tOrgans arrived:\n", statistics->numOrganArrivals, stdout)
    print_by_blood_type("\tOrgans outdated:\n", statistics->numOrganOutdatings, stdout)
    print_transplants_res("\tTransplants:\n", statistics->numTransplants, stdout)
    print_by_blood_type("\tOrgans in queue:\n", statistics->numOrgans, stdout)
    print_by_all("\tPatients in queue:\n", statistics->numPatients, stdout)
}

void cleanUp(event_list* events){
    patient_waiting_list *waiting_list = &events->patient_arrival;
    patient_queue_blood_type **bt_queues = waiting_list->blood_type_queues;
    organ_bank *bank = &events->organ_arrival;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(bank->queues[i]->queue); //TODO: while there are organs in queue, free organ
        free(bank->queues[i]);

        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            free(bt_queues[i]->priority_queue[j]->queue); //TODO: while there are patients in queue, free patient
            free(bt_queues[i]->priority_queue[j]);
        }

        free(bt_queues[i]);
    }

    transplant *trans_queue = &events->transplant_arrival;
    free(trans_queue->transplanted_patients); //TODO: while there are patients in queue, free patient

    activation *inactive_queue = &events->activation_arrival;
    free(inactive_queue->inactive_patients->patient); //TODO: while there are patients in queue, free patient
    free(inactive_queue->inactive_patients);

    //TODO: free the other structured into events
}
