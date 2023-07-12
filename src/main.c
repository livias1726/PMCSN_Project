#include <stdint.h>
#include "headers/utils.h"

#define START 0.0                      /* initial sim_time                   */
#define STOP 2190                      /* terminal (close the door) sim_time - 365*6 */
#define INFINITE   (100.0 * STOP)      /* must be much larger than STOP  */

double simulation_time = START;


int main(){

    // ---------------------------------------------- Intro --------------------------------------------------------

    printf("NICE HEADER IF WE WANT\n");
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
    printf("Simulation %s %s model\n\n", simulation, model);

    // ------------------------------------------------------- Initialization --------------------------------------

    event_list events = initialize_event_list();
    sim_time simTime = initialize_time();
    activation activation_c = initialize_activation_center();

    events.organArrival.interArrivalTime[O] = getOrganArrival(O, START);
    events.organArrival.interArrivalTime[A] = getOrganArrival(A, START);
    events.organArrival.interArrivalTime[B] = getOrganArrival(B, START);
    events.organArrival.interArrivalTime[AB] = getOrganArrival(AB, START);

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events.patientArrival.interArrivalTime[i][j] = getPatientArrival(i, j, START);
        }
    }

    events.organsLoss.renegingTime[O] = INFINITY;
    events.organsLoss.renegingTime[A] = INFINITY;
    events.organsLoss.renegingTime[B] = INFINITY;
    events.organsLoss.renegingTime[AB] = INFINITY;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (int j = 0; j < NUM_PRIORITIES; ++j) {
            events.patientsLoss.renegingTime[i][j] = INFINITY;
            events.patientsLoss.deathTime[i][j] = INFINITY;
        }
    }

    /* Choose next event selecting minimum time */


    // ----------------------------------------------------- Test -----------------------------------------------------

    int patients_arrived = 0;
    int organs_arrived = 0;

    srand(time(NULL));   // Initialization, should only be called once.
    int r_event = rand(), r_bt = rand(), r_pr = rand();

    for (int i = 0; i < 100; ++i) {
        switch (r_event % 5) {
            case 0:
                handlePatientArrival(&events, &simTime, r_bt % NUM_BLOOD_TYPES, r_pr % NUM_PRIORITIES);
                patients_arrived++;
                break;
            case 1:
                handleOrganArrival(&events, &simTime, r_bt % NUM_BLOOD_TYPES);
                organs_arrived++;
                break;
            case 3:
                handleOrganRenege(&events, &simTime, r_bt % NUM_BLOOD_TYPES);
                break;
            case 4:
                handlePatientLoss(&events, &simTime, renege, r_bt % NUM_BLOOD_TYPES, r_pr % NUM_PRIORITIES);
                break;
        }

        handleMatching(&events);
        r_event = rand();
        r_bt = rand();
        r_pr = rand();
    }

    /* TEST: EVENT: new patient arrival
     *  -> test with: (14 critical, 20 normal, 50 low) per each blood type
     *  -> total: 56 critical, 80 normal, 200 low
     *  -> 336
     *  */
    /*
    for (int b = 0; b < NUM_BLOOD_TYPES; ++b) {
        for (int i = 0; i < 14; ++i) {
            handlePatientArrival(b, critical, &waiting_list, &bank);
        }
        for (int i = 0; i < 20; ++i) {
            handlePatientArrival(b, normal, &waiting_list, &bank);
        }
        for (int i = 0; i < 50; ++i) {
            handlePatientArrival(b, low, &waiting_list, &bank);
        }
    }
     */

    /* TEST: EVENT: new organ arrival
     *  -> test with: 6 organs per each blood type
     *  -> total: 24 organs
     *  */
    /*
    for (int i = 0; i < 6; ++i) {
        handleOrganArrival(O, &waiting_list, &bank);
        handleOrganArrival(A, &waiting_list, &bank);
        handleOrganArrival(AB, &waiting_list, &bank);
        handleOrganArrival(B, &waiting_list, &bank);
    }
     */
    /*
    for (int i = 0; i < 6; ++i) {
        handleOrganArrival(O, &waiting_list, &bank);
        handleOrganArrival(A, &bank);
        handleOrganArrival(AB, &bank);
        handleOrganArrival(B, &bank);
    }
     */

    /*organ_queue *queue = bank.queues[0];
    removeOrgan(2, &queue);*/

    /* organ renege */
    /*handleOrganRenege(O, &bank);
    handleOrganRenege(A, &bank);
    handleOrganRenege(B, &bank);
    handleOrganRenege(AB, &bank);*/

    /* Matching
    time_t start, end;
    start = clock();

    //handleMatching(ABO_Id, &waiting_list, &bank);
    //handleMatching(ABO_Comp, &waiting_list, &bank);

    end = clock();
    time_t res = (end - start) / CLOCKS_PER_SEC;
    */

    /* patient death */
    //handlePatientDeath(O, low, &waiting_list);
    /* patient renege */
    //handlePatientRenege(O, low, &waiting_list);
    //handlePatientRenege(O, low, &waiting_list);
    //handlePatientRenege(O, low, &waiting_list);
    /* organ renege */
    //handleOrganRenege(O, &bank);

    /*
    printf("Test should have an outcome of 0 organs in queue and 312 patients in queue\n");
    printf("Results: \n"
           "\tOrgans in queue: %f\n"
           "\tPatients in queue: %f\n", bank.total_number, waiting_list.total_number);
*/

    patient_waiting_list waiting_list = events.patientArrival;
    organ_bank bank = events.organArrival;
    transplant transplant_c = events.transplantArrival;
    patients_lost patient_loss = events.patientsLoss;
    organs_expired organs_loss = events.organsLoss;

    printf("Executing a random test\n");
    printf("Results: \n");
    printf("\tPatients arrived: %d\n"
           "\tOrgans arrived: %d\n", patients_arrived, organs_arrived);
    printf("\tOrgans transplanted: %d\n", (int)transplant_c.total_number);
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

    // Setup
    if ((stats_file = fopen("data/fhStats.txt", "w+")) == NULL
            || (samples_file = fopen("data/fhSamples.txt", "w+")) == NULL){
        fprintf(stderr, "Cannot open file\n");
        exit(-1);
    }

    out_stats = malloc(sizeof(stats));
    MALLOC_HANDLER(out_stats)

    PlantSeeds(SEED);

    // Iterations
    for (int i = 1; i <= NUM_ITER; ++i){
        // TODO: call the FH simulation for iteration i -> input: samples, output: out_stats

        compute_stats();
    }

    //print_stats();

    fclose(stats_file);
    fclose(samples_file);
#else
    // TODO: call the IH simulation
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

