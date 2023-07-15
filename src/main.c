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

    /* Choose next event selecting minimum time */
    int patients_arrived_c = 0;
    int patients_arrived_n = 0;
    int patients_arrived_l = 0;
    int organs_arrived = 0;

    sim(&events, &simTime, &organs_arrived, &patients_arrived_c, &patients_arrived_n, &patients_arrived_l);

#ifdef FINITE_HORIZON
    finite_sim(&events, &simTime);
#else
    /* TODO */
#endif

    // ----------------------------------------------------- Test -----------------------------------------------------

    patient_waiting_list waiting_list = events.patientArrival;
    organ_bank bank = events.organArrival;
    transplant transplant_c = events.transplantArrival;
    activation activation_c = events.activationArrival;
    patients_lost patient_loss = events.patientsLoss;
    organs_expired organs_loss = events.organsLoss;

    int patients_dead_n = 0;
    int patients_dead_c = 0;
    int patients_dead_l = 0;
    int patient_reneged = 0;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            switch (j) {
                case critical:
                    patients_dead_c += (int)patient_loss.number_dead[i][j];
                case normal:
                    patients_dead_n += (int)patient_loss.number_dead[i][j];
                case low:
                    patients_dead_l += (int)patient_loss.number_dead[i][j];
                default:
                    break;
            }
            patient_reneged += (int)patient_loss.number_renege[i][j];
        }
    }

    printf("Executing a random test\n");
    printf("Results: \n");
    printf("\tPatients arrived: \n"
           "\t\tcrit: %d\n"
           "\t\tnormal: %d\n"
           "\t\tlow: %d\n"
           "\tOrgans arrived: %d\n", patients_arrived_c, patients_arrived_n, patients_arrived_l, organs_arrived);
    printf("\tPatients dead: \n"
           "\t\tcrit: %d\n"
           "\t\tnormal: %d\n"
           "\t\tlow: %d\n"
           "\tPatients reneging: %d\n", patients_dead_c, patients_dead_n, patients_dead_l, patient_reneged);
    printf("\tTransplants: \n"
           "\t\tsuccessful: %d\n"
           "\t\trejected: %d\n", (int)transplant_c.completed_transplants, (int)transplant_c.rejected_transplants);
    printf("\tOrgans lost: \n"
           "\t\tO: %f\n"
           "\t\tA: %f\n"
           "\t\tB: %f\n"
           "\t\tAB: %f\n", organs_loss.number[O], organs_loss.number[A], organs_loss.number[B], organs_loss.number[AB]);
    printf("\tOrgans in queue: \n"
           "\t\tO: %f\n"
           "\t\tA: %f\n"
           "\t\tB: %f\n"
           "\t\tAB: %f\n", bank.queues[O]->number, bank.queues[A]->number, bank.queues[B]->number, bank.queues[AB]->number);

    printf("\tPatients in queue: \n"
           "\t\tO: \n"
           "\t\t\tCritical: %f\n"
           "\t\t\tNormal: %f\n"
           "\t\t\tLow: %f\n", waiting_list.blood_type_queues[O]->priority_queue[critical]->number,
                                waiting_list.blood_type_queues[O]->priority_queue[normal]->number,
                                waiting_list.blood_type_queues[O]->priority_queue[low]->number);
    printf("\t\tA: \n"
           "\t\t\tCritical: %f\n"
           "\t\t\tNormal: %f\n"
           "\t\t\tLow: %f\n", waiting_list.blood_type_queues[A]->priority_queue[critical]->number,
           waiting_list.blood_type_queues[A]->priority_queue[normal]->number,
           waiting_list.blood_type_queues[A]->priority_queue[low]->number);
    printf("\t\tB: \n"
           "\t\t\tCritical: %f\n"
           "\t\t\tNormal: %f\n"
           "\t\t\tLow: %f\n", waiting_list.blood_type_queues[B]->priority_queue[critical]->number,
           waiting_list.blood_type_queues[B]->priority_queue[normal]->number,
           waiting_list.blood_type_queues[B]->priority_queue[low]->number);
    printf("\t\tAB: \n"
           "\t\t\tCritical: %f\n"
           "\t\t\tNormal: %f\n"
           "\t\t\tLow: %f\n", waiting_list.blood_type_queues[AB]->priority_queue[critical]->number,
           waiting_list.blood_type_queues[AB]->priority_queue[normal]->number,
           waiting_list.blood_type_queues[AB]->priority_queue[low]->number);


    // ---------------------------------------------------- Simulation -----------------------------------------------

#ifdef FINITE_HORIZON
    stats *out_stats = NULL; // doubles should be zero-initialized by default
    FILE *stats_file, *samples_file;

    stats *statistics = initializeStatistics();

    gather_results(statistics, &events);

#ifdef AUDIT
    print_results(statistics);
#else
    save_results(statistics);
#endif

    // ----------------------------------------------- Clean up -----------------------------------------------------

    CLEANUP(NUM_BLOOD_TYPES, bank.queues)
    for (int b = 0; b < NUM_BLOOD_TYPES; ++b) {
        CLEANUP(NUM_PRIORITIES, waiting_list.blood_type_queues[b]->priority_queue)
    }
    CLEANUP(NUM_BLOOD_TYPES, waiting_list.blood_type_queues)

    void* tmp[] = {&bank, &waiting_list, &activation_c, &transplant_c};
    //CLEANUP(4, tmp)
}

#endif
}
