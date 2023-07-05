#include "headers/utils.h"

#define START 0.0                      /* initial time                   */
#define STOP 2190                      /* terminal (close the door) time - 365*6 */
#define INFINITE   (100.0 * STOP)      /* must be much larger than STOP  */

double simulation_time = START;



// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initialize_waiting_list() {
    patient_waiting_list waitingList;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waitingList.blood_type_queues[i] = NULL;
    }
    waitingList.total_number = 0.0;
    return waitingList;
}

organ_bank initialize_organ_bank() {
    organ_bank organBank;
    for (int i = 0; i < NUM_ORGAN_QUEUES; ++i) {
        organBank.queues[i] = NULL;
    }
    organBank.total_number = 0.0;
    return organBank;
}

transplant initialize_transplant_center() {
    transplant transplantCenter;
    transplantCenter.total_number = 0.0;
    transplantCenter.serviceTime = 0.0;
    return transplantCenter;
}

activation initialize_activation_center() {
    activation activationCenter;
    activationCenter.total_number = 0.0;
    activationCenter.serviceTime = 0.0;
    return activationCenter;
}

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
    handleMatching(ABO_Comp, &waiting_list, &bank);
    //handleMatching(ABO_Id, &waiting_list, &bank);

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

/*  clean_up(sizeof(organ_bank), (void **) &bank);
    clean_up(sizeof(patient_waiting_list), (void **) &waiting_list);
    clean_up(sizeof(activation), (void **) &activation_c);
    clean_up(sizeof(transplant), (void **) &transplant_c);
*/

    printf("%f\n", bank.total_number);
    printf("%f\n", waiting_list.total_number);

    for (int i = 0; i < NUM_ORGAN_QUEUES; ++i) {
        free(bank.queues[i]);
    }
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(waiting_list.blood_type_queues[i]);
    }


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

    out_stats = (stats*) malloc(sizeof(stats));
    malloc_handler(1, (void*[]){out_stats});

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

