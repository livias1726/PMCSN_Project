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

    event_list events = initializeEventList();
    sim_time simTime = initializeTime();
    initializeEventTime(&events);

    // --------------------------------------------- Simulation ------------------------------------------------------

#ifdef FINITE_HORIZON
    finiteSim(&events, &simTime);
#else
    /* TODO */
#endif

    // ----------------------------------------------------- Results --------------------------------------------------

    stats *statistics = initializeStatistics();
    gatherResults(statistics, &events);
    computeStats(statistics);

#ifdef AUDIT
    printResults(statistics);
#else
    saveResultsCsv(statistics);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------

    cleanUp(&events);
}

