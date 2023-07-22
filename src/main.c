#include "headers/utils.h"

int main(){

    // ---------------------------------------------- Intro --------------------------------------------------------

    char *simulation, *model, *policy;

#ifdef FINITE_HORIZON
    simulation = "Finite";
#else
    simulation = "Infinite";
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

    printf("Simulation: %s horizon - %s model (Policy: ABO %s)\n\n", simulation, model, policy);

    // -------------------------------------------- Initialization ---------------------------------------------------

    PlantSeeds(123456789);

    event_list *events = initializeEventList();
    initializeEventTime(events);

    sim_time sim_time = initializeTime();
    time_integrated_stats *ti_stats = initializeTimeStatistics();

    // --------------------------------------------- Simulation ------------------------------------------------------

    init_state *state = malloc(sizeof(init_state));
    MALLOC_HANDLER(state)

#ifdef FINITE_HORIZON
    finiteSim(events, &sim_time, ti_stats);
#else
    /* TODO */
#endif

    // ----------------------------------------------------- Results --------------------------------------------------
    stats *statistics = initializeStatistics();

    gatherResults(statistics, events);
    computeTimeAveragedStats2(statistics, ti_stats);

#ifdef AUDIT
    printResults(statistics, stdout);
#else
    saveResultsCsv(statistics);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------

    cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    cleanUpStatistics(statistics);
}

