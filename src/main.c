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

    event_list events = initialize_event_list();
    sim_time simTime = initialize_time();
    initializeEventTime(&events);

    stats *statistics = malloc(sizeof(stats));
    MALLOC_HANDLER(statistics)
    initializeStatistics(statistics);

    // --------------------------------------------- Simulation ------------------------------------------------------

#ifdef FINITE_HORIZON
    finite_sim(&events, &simTime, statistics);
#else
    /* TODO */
#endif

    // ----------------------------------------------------- Test -----------------------------------------------------

    patient_waiting_list waiting_list = events.patientArrival;
    organ_bank bank = events.organArrival;
    transplant transplant_c = events.transplantArrival;
    activation activation_c = events.activationArrival;

    gather_results(statistics, waiting_list, bank, transplant_c);

#ifdef AUDIT
    print_results(statistics);
#else
    save_results(statistics);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------

    CLEANUP(NUM_ORGAN_QUEUES, bank.queues)
    for (int b = 0; b < NUM_BLOOD_TYPES; ++b) {
        CLEANUP(NUM_PRIORITIES, waiting_list.blood_type_queues[b]->priority_queue)
    }
    CLEANUP(NUM_BLOOD_TYPES, waiting_list.blood_type_queues)

    void* tmp[] = {&bank, &waiting_list, &activation_c, &transplant_c};
    //CLEANUP(4, tmp)
}

