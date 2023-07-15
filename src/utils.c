#include "headers/utils.h"

void compute_output_stats(int i, stats* mean, stats* variance, stats* out_stats, stats* conf_mean){
    double aux = (i-1)/(double)i;
/* TODO: separate computations if the improvement affects params
#ifndef IMPROVEMENT
    ...
#else
    ...
#endif
 */

/*
    // compute the mean for each param and each node considered
    mean->param_node += GET_MEAN(out_stats->param_node, mean->param_node, i);

    // compute the variance for each param and each node considered
    variance->param_node += GET_VAR(out_stats->param_node, mean->param_node, aux);

    // compute the confidence interval for each param and each node considered
    double criticalValue = idfStudent(i-1,1-0.025); //alpha = 0.05
    conf_mean->param_node = GET_CONF(variance->param_node, i, criticalValue);
*/
}

void save_output_stats(int i, stats *mean, stats* conf_mean, FILE *stats_file){
/* TODO: separate computations if the improvement affects params
#ifndef IMPROVEMENT
    ...
#else
    ...
#endif
 */

/*
    if (i == 1){
        fprintf(stats_file, "%d,%f,%f\n", i, mean->param_node, 0.0);
    } else {
        fprintf(stats_file, "%d,%f,%f\n", i, mean->param_node, conf_mean->param_node);
    }
*/
}

void gather_results(stats* statistics, patient_waiting_list waiting_list, organ_bank bank, transplant transplant_c){
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        statistics->numOrgans[i] = (int)bank.queues[i]->number;
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            statistics->numPatients[i*NUM_PRIORITIES + j] = (int)waiting_list.blood_type_queues[i]->priority_queue[j]->number;
        }
    }

    statistics->numTransplants[0] = (int)transplant_c.completed_transplants;
    statistics->numTransplants[0] = (int)transplant_c.rejected_transplants;
}

void save_results(stats* statistics){
    FILE *f;
    char path[PATH_MAX];
#ifdef ABO_ID
    snprintf(path, PATH_MAX, "output/res_%s.txt", "id");
#else
    snprintf(path, PATH_MAX, "output/res_%s.txt", "comp");
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
}

void print_results(stats* statistics){
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

void print_output_stats(stats* mean, stats* variance, stats* out_stats){

  //  double criticalValue = idfStudent(NUM_ITER-1,1-0.025); //alpha = 0.05

    printf("OUTPUT:\n");

/* TODO: separate printing if the improvement affects params
#ifndef IMPROVEMENT
    ...
#else
    ...
#endif
 */

  //  printf("\t<Param> <Node>: %lf +_ %lf \n", mean->param_node, GET_CONF(variance->param_node, NUM_ITER, criticalValue));
}
