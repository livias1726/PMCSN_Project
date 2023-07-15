
/*
void print_single_events(){

}

void test_single_events() {
    srand(time(NULL));
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

    print_single_events();
}

void print_arrivals_result(organ_bank bank, patient_waiting_list waiting_list, int expOrgans, int expPatients){
    printf("Test should have an outcome of %d organs in queue and %d patients in queue\n",
           expOrgans, expPatients);

    printf("Results: \n"
           "\tOrgans in queue: %f\n"
           "\tPatients in queue: %f\n", bank.total_number, waiting_list.total_number);
}
*/

//void test_arrivals(patient_waiting_list *waiting_list, organ_bank *bank){

    /* TEST: EVENT: new patient arrival
     *  -> test with: (14 critical, 20 normal, 50 low) per each blood type
     *  -> total: 56 critical, 80 normal, 200 low
     *  -> 336
     *
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
    }*/

    /* TEST: EVENT: new organ arrival
     *  -> test with: 6 organs per each blood type
     *  -> total: 24 organs
     *
    for (int i = 0; i < 6; ++i) {
        handleOrganArrival(O, &waiting_list, &bank);
        handleOrganArrival(A, &waiting_list, &bank);
        handleOrganArrival(AB, &waiting_list, &bank);
        handleOrganArrival(B, &waiting_list, &bank);
    }*/
/*
    // Matching
    time_t start, end;
    start = clock();

    //handleMatching(ABO_Id, &waiting_list, &bank);
    //handleMatching(ABO_Comp, &waiting_list, &bank);

    end = clock();
    time_t res = (end - start) / CLOCKS_PER_SEC;

    //print_arrivals_result(*bank, *waiting_list, 0, 312);
}*/
