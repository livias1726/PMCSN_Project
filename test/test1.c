
class test(){

    /* TEST: EVENT: new patient arrival
     *  -> test with: (14 critical, 20 normal, 50 low) per each blood type
     *  -> total: 56 critical, 80 normal, 200 low
     *  -> 336
     *  */
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

    /* TEST: EVENT: new organ arrival
     *  -> test with: 6 organs per each blood type
     *  -> total: 24 organs
     *  */
    for (int i = 0; i < 6; ++i) {
        handleOrganArrival(O, &waiting_list, &bank);
        handleOrganArrival(A, &waiting_list, &bank);
        handleOrganArrival(AB, &waiting_list, &bank);
        handleOrganArrival(B, &waiting_list, &bank);
    }

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
}
