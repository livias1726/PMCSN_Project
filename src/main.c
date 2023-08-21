#include "headers/utils.h"
#include "headers/sim.h"
#include "headers/stats.h"
#include "headers/init.h"
#include "headers/io.h"
#include "headers/cleanup.h"

int main(){

    char *model, *policy, *sim;
    event_list *events;
    sim_time sim_time;
    time_integrated_stats *ti_stats;
    int i, num_iter;
    stats **batches = NULL, *final_stat;
    time_t s, e;

    // ---------------------------------------------- Intro --------------------------------------------------------

#ifdef FINITE
    sim = "Finite";
    stats *mid_stats[ITERATIONS];
#else
    sim = "Infinite";
#endif

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

    printf("Simulation: %s horizon - %s model (Policy: ABO %s)\n\n", sim, model, policy);

    // --------------------------------------------- Initialization --------------------------------------------------

    events = initializeEventList();
    initializeEventTime(events);

    sim_time = initializeTime();
    ti_stats = initializeTimeStatistics();

#ifdef FINITE
    // --------------------------------------------- Finite simulation -----------------------------------------------
    PlantSeeds(SEED);

    for (i = 0; i < ITERATIONS; ++i) {
        mid_stats[i] = initializeStatistics();

        s = clock();
        finiteSim(events, &sim_time, ti_stats, batches, mid_stats[i], &num_iter);
        e = clock();
        printf("Simulation %d - time: %lld\n", i, (e-s)/CLOCKS_PER_SEC);

        computeStatistics(mid_stats[i], num_iter);
        //saveResultsCsv(i, mid_stats[i], false, 0);
        //saveResultsLean(mid_stats[i]);

        resetSimulation(events, &sim_time, ti_stats);
    }

    final_stat = initializeStatistics();
    computeStatistics(final_stat, ITERATIONS);
#else
    // --------------------------------------------- Infinite simulation ---------------------------------------------

    // batches for each observation year
    num_iter = (STOP / BATCH_SIZE) + 1;
    batches = malloc(num_iter * sizeof(stats*));
    MALLOC_HANDLER(batches)
    for (i = 0; i < num_iter; ++i) {
        batches[i] = initializeStatistics();
    }
    final_stat = initializeStatistics();

    PlantSeeds(SEED);

    s = clock();
    infiniteSim(events, &sim_time, ti_stats, batches, final_stat, &num_iter);
    e = clock();
    printf("Simulation time: %lld seconds\n", (e-s)/CLOCKS_PER_SEC);

#endif

    // ----------------------------------------------------- Results --------------------------------------------------

    computeStatistics(final_stat, ITERATIONS);

    saveResultsCsv(0, final_stat, false, 0);
    //saveResultsLean(final_stat);

    // ----------------------------------------------- Clean up -----------------------------------------------------

    //cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    for (i = 0; i < num_iter; ++i) {
        cleanUpStatistics(batches[i]);
    }
    cleanUpStatistics(final_stat);
}
