#include "headers/utils.h"

int main(){

    // ---------------------------------------------- Intro --------------------------------------------------------

    char *model, *policy;

#ifdef IMPROVEMENT
    model = "Improved";
    policy = "Incompatible";
#else
    model = "Base";
    #ifdef ABO_ID
        policy = "Identical";
    #else
        policy = "Compatible";
    #endif
#endif

    printf("Simulation: Finite horizon - %s model (Policy: ABO %s)\n\n", model, policy);

    // -------------------------------------------- Initialization ---------------------------------------------------

    PlantSeeds(123456789);

    event_list *events = initializeEventList();
    initializeEventTime(events);

    sim_time sim_time = initializeTime();
    time_integrated_stats *ti_stats = initializeTimeStatistics();

    // statistics for each observation year
    int i, num_iter = OBSERVATION * (365 / BATCH_SIZE); // measure for each month
    stats **statistics = malloc(num_iter* sizeof(stats*));
    for (i = 0; i < num_iter; ++i) {
        statistics[i] = initializeStatistics();
    }

    // --------------------------------------------- Simulation ------------------------------------------------------

    time_t s, e;
    s = clock();

    finiteSim(events, &sim_time, ti_stats, statistics);

    e = clock();
    printf("time: %lld\n", (e-s)/CLOCKS_PER_SEC);

    // ----------------------------------------------------- Results --------------------------------------------------

    stats * batch = computeFinalStatistics(statistics, num_iter);

#ifdef AUDIT
    printResults(statistics, stdout);
#else
    saveResultsCsv(batch);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------

    cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    for (i = 0; i < num_iter; ++i) {
        cleanUpStatistics(statistics[i]);
    }
    cleanUpStatistics(batch);

}

