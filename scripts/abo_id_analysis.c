#include <stdio.h>
#include <stdlib.h>

#define AUDIT if(0)

/**
 * This script will compute and save in a XLSX file a 10 year analytical simulation for ABO identical.
 * */

typedef struct metrics {
    double s;
    double ro;
    double tq;
    double ts;
    double nq;
    double ns;
} metrics_t;

void read_rates(char* file, double *p, double* o){
    FILE* f;
    if((f = fopen(file, "r")) == NULL){
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while(count < 8){
        fscanf(f, "%lf\n", (p+count));
        count++;
    }

    count = 0;
    while(count < 4){
        fscanf(f, "%lf\n", (o+count));
        count++;
    }
}

void print_rates(double *rates, int size){
    printf("[");
    for (int i = 0; i < size-1; ++i) printf("%f, ", rates[i]);
    printf("%f]\n", rates[size-1]);
}

void populate_metrics(metrics_t *m, double *p, double* o){

}

int main(){
    // read rates from file and set them in 2 arrays: lambda_eff_p, lambda_o
    double l_p[8], l_o[4];
    read_rates("rates.txt", l_p, l_o);
    AUDIT{
        print_rates(l_p, 8);
        print_rates(l_o, 4);
    }

    // compute performance metrics
    metrics_t pm[8];
    populate_metrics(pm, l_p, l_o);
}