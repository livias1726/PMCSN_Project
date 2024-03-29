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
    int i;
    stats *final_stat;
    time_t s, e;

    // ---------------------------------------------- Intro --------------------------------------------------------

#ifdef FINITE
    sim = "Finite";
    stats *mid_stats[ITERATIONS];
#else
    int num_iter;
    stats **batches = NULL;
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
    final_stat = initializeStatistics();

    PlantSeeds(SEED);

#ifdef FINITE
    // --------------------------------------------- Finite simulation -----------------------------------------------

    for (i = 0; i < ITERATIONS; ++i) {
        mid_stats[i] = initializeStatistics();

        s = clock();
        finiteSim(events, &sim_time, ti_stats, mid_stats[i]);
        e = clock();
        printf("Simulation %d - time: %ld\n", i, (e-s)/CLOCKS_PER_SEC);

        gatherResults(mid_stats[i], mid_stats[i-1], events, i);
        computeTimeAveragedStats(mid_stats[i], ti_stats, &sim_time);
        welford(i+1, final_stat, mid_stats[i]);
#ifdef AUDIT
        printf("Arrivals O - crit: %f\n", final_stat->wl_stats->avg_arrivals[O][critical]);
        printf("Arrivals O - norm: %f\n\n", final_stat->wl_stats->avg_arrivals[O][normal]);
#endif
        resetSimulation(events, &sim_time, ti_stats);
    }

    computeStatistics(final_stat, ITERATIONS);
    saveResultsCsv(i, final_stat, false, 0);
    saveResultsLean(final_stat);
#else
    // --------------------------------------------- Infinite simulation ---------------------------------------------

    // batches for each observation year
    num_iter = INF_ITER;
    batches = malloc(num_iter * sizeof(stats*));
    MALLOC_HANDLER(batches)
    for (i = 0; i < num_iter; ++i) {
        batches[i] = initializeStatistics();
    }

    s = clock();
    infiniteSim(events, &sim_time, ti_stats, batches, final_stat, &num_iter);
    e = clock();
    printf("Simulation time: %ld seconds\n", (e-s)/CLOCKS_PER_SEC);

    computeStatistics(final_stat, num_iter);
    saveResultsCsv(0, final_stat, false, 0);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------
#ifndef FINITE
    for (i = 0; i < num_iter; ++i) {
        cleanUpStatistics(batches[i]);
    }
#endif
    cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    cleanUpStatistics(final_stat);
}
