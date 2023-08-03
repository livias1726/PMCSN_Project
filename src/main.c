#include "headers/utils.h"
#include "headers/sim.h"
#include "headers/stats.h"
#include "headers/init.h"

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

    PlantSeeds(SEED);

    event_list *events = initializeEventList();
    initializeEventTime(events);

    sim_time sim_time = initializeTime();
    time_integrated_stats *ti_stats = initializeTimeStatistics();

    // batches for each observation year
    int i, num_iter = (STOP / BATCH_SIZE)+1; // measure for each month
    stats **batches = malloc(num_iter * sizeof(stats*));
    MALLOC_HANDLER(batches)
    for (i = 0; i < num_iter; ++i) {
        batches[i] = initializeStatistics();
    }

    stats *final_stat = initializeStatistics();

    // --------------------------------------------- Simulation ------------------------------------------------------

    time_t s, e;
    s = clock();

    finiteSim(events, &sim_time, ti_stats, batches, final_stat, &num_iter);

    e = clock();
    printf("time: %lld\n", (e-s)/CLOCKS_PER_SEC);

    // ----------------------------------------------------- Results --------------------------------------------------

    computeFinalStatistics(final_stat, batches, num_iter);

    saveResultsCsv(final_stat, false, 0);

    // ----------------------------------------------- Clean up -----------------------------------------------------

    cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    for (i = 0; i < num_iter; ++i) {
        cleanUpStatistics(batches[i]);
    }
    cleanUpStatistics(final_stat);

}

