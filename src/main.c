#include "headers/utils.h"

#define START 0.0                      /* initial time                   */
#define STOP 2190                      /* terminal (close the door) time - 365*6 */
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
    patient_waiting_list waiting_list = initialize_waiting_list();
    organ_bank bank = initialize_organ_bank();
    activation activation_c = initialize_activation_center();
    transplant transplant_c = initialize_transplant_center();

    /* new organ arrival */
    for (int i = 0; i < 6; ++i) {
        handleOrganArrival(O, &bank);
        handleOrganArrival(A, &bank);
        handleOrganArrival(AB, &bank);
        handleOrganArrival(B, &bank);
    }

    /*organ_queue *queue = bank.queues[0];
    removeOrgan(2, &queue);*/

    /* organ renege */
    /*handleOrganRenege(O, &bank);
    handleOrganRenege(A, &bank);
    handleOrganRenege(B, &bank);
    handleOrganRenege(AB, &bank);*/

    /* new patient arrival */
    /*for (int b = 0; b < NUM_BLOOD_TYPES; ++b) {
        for (int i = 0; i < 14; ++i) {
            handlePatientArrival(b, critical, &waiting_list);
        }
        for (int i = 0; i < 20; ++i) {
            handlePatientArrival(b, normal, &waiting_list);
        }
        for (int i = 0; i < 50; ++i) {
            handlePatientArrival(b, low, &waiting_list);
        }
    }*/
    for (int i = 0; i < 14; ++i) {
        handlePatientArrival(O, critical, &waiting_list);
    }
    for (int i = 0; i < 20; ++i) {
        handlePatientArrival(O, normal, &waiting_list);
    }
    for (int i = 0; i < 50; ++i) {
        handlePatientArrival(O, low, &waiting_list);
    }

    /* Matching - ABOIdentical */
    //handleMatching(ABO_Comp, &waiting_list, &bank);
    handleMatching(ABO_Id, &waiting_list, &bank);

    /* patient death */
    //handlePatientDeath(O, low, &waiting_list);
    /* patient renege */
    //handlePatientRenege(O, low, &waiting_list);
    //handlePatientRenege(O, low, &waiting_list);
    //handlePatientRenege(O, low, &waiting_list);
    /* organ renege */
    //handleOrganRenege(O, &bank);

    /*bool res;
    res = ABOIdentical(O, O)==true;
    printf("%b\n", res);
    res = ABOIdentical(O, A)==false;
    printf("%b\n", res);
    res = ABOIdentical(O, AB)==false;
    printf("%b\n", res);
    res = ABOIdentical(O, B)==false;
    printf("%b\n", res);
    res = ABOIdentical(A, O)==false;
    printf("%b\n", res);
    res = ABOIdentical(A, A)==true;
    printf("%b\n", res);
    res = ABOIdentical(A, B)==false;
    printf("%b\n", res);
    res = ABOIdentical(A, AB)==false;
    printf("%b\n", res);
    res = ABOIdentical(B, O)==false;
    printf("%b\n", res);
    res = ABOIdentical(B, A)==false;
    printf("%b\n", res);
    res = ABOIdentical(B, B)==true;
    printf("%b\n", res);
    res = ABOIdentical(B, AB)==false;
    printf("%b\n", res);
    res = ABOIdentical(AB, O)==false;
    printf("%b\n", res);
    res = ABOIdentical(AB, A)==false;
    printf("%b\n", res);
    res = ABOIdentical(AB, B)==false;
    printf("%b\n", res);
    res = ABOIdentical(AB, AB)==true;
    printf("%b\n", res);*/

    /* ABO compatible */
   /* res = ABOCompatible(O, O)==true;
    printf("%b\n", res);
    res = ABOCompatible(O, A)==true;
    printf("%b\n", res);
    res = ABOCompatible(O, AB)==true;
    printf("%b\n", res);
    res = ABOCompatible(O, B)==true;
    printf("%b\n", res);
    res = ABOCompatible(A, O)==false;
    printf("%b\n", res);
    res = ABOCompatible(A, A)==true;
    printf("%b\n", res);
    res = ABOCompatible(A, B)==false;
    printf("%b\n", res);
    res = ABOCompatible(A, AB)==true;
    printf("%b\n", res);
    res = ABOCompatible(B, O)==false;
    printf("%b\n", res);
    res = ABOCompatible(B, A)==false;
    printf("%b\n", res);
    res = ABOCompatible(B, B)==true;
    printf("%b\n", res);
    res = ABOCompatible(B, AB)==true;
    printf("%b\n", res);
    res = ABOCompatible(AB, O)==false;
    printf("%b\n", res);
    res = ABOCompatible(AB, A)==false;
    printf("%b\n", res);
    res = ABOCompatible(AB, B)==false;
    printf("%b\n", res);
    res = ABOCompatible(AB, AB)==true;
    printf("%b\n", res);*/

    printf("%f\n", bank.total_number);
    printf("%f\n", waiting_list.total_number);

    /* TODO: changed cleanup in a macro that handles those as well
    for (int i = 0; i < NUM_ORGAN_QUEUES; ++i) {
        free(bank.queues[i]);
    }
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(waiting_list.blood_type_queues[i]);
    }
     */

    //TODO: NON SO SE FUNZIONERA'
    CLEANUP(NUM_ORGAN_QUEUES, bank.queues)
    CLEANUP(NUM_BLOOD_TYPES, waiting_list.blood_type_queues)

    void ** tmp = {&bank, &waiting_list, &activation_c, &transplant_c};
    CLEANUP(4, tmp)


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

}

