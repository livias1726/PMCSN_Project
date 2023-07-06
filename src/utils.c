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
