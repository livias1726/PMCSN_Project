#include "headers/utils.h"

int main(){
    // ---------------------------------------------- Intro --------------------------------------------------------
    printf("NICE HEADER IF WE WANT");
    char *simulation, *model;
#ifdef FINITE_HORIZON
    simulation = "Finite horizon";
#else
    simulation = "Infinite horizon";
#endif

#ifdef IMPROVEMENT
    model = "Improved"
#else
    model = "Base";
#endif
    printf("%s simulation\n%s model\n\n", simulation, model);

    // ---------------------------------------------------- Simulation -----------------------------------------------
#ifdef FINITE_HORIZON
    STATS *mean, *variance, *out_stats, *conf_mean; // doubles should be zero-initialized by default
    FILE *stats_file, *samples_file;

    // Setup
    if ((stats_file = fopen("data/fhStats.txt", "w+")) == NULL
            || (samples_file = fopen("data/fhSamples.txt", "w+")) == NULL){
        fprintf(stderr, "Cannot open file\n");
        exit(-1);
    }

    mean = (STATS*) malloc(sizeof(STATS));
    variance = (STATS*) malloc(sizeof(STATS));
    out_stats = (STATS*) malloc(sizeof(STATS));
    conf_mean = (STATS*) malloc(sizeof(STATS));
    malloc_handler(4, (void*[]){mean, variance, out_stats, conf_mean});

    PlantSeeds(123456789);

    // Iterations
    for (int i = 1; i <= NUM_ITER; ++i){
        // TODO: call the FH simulation for iteration i -> input: samples, output: out_stats

        compute_output_stats(i, mean, variance, out_stats, conf_mean);
        save_output_stats(i, mean, conf_mean, stats_file);
    }

    print_output_stats(mean, variance, out_stats);

    fclose(stats_file);
    fclose(samples_file);

#else
    // TODO: call the IH simulation
#endif

}
