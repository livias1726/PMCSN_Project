/* Internal usage:
 *      Upon the arrival of a new patient, this function search for a compatible organ in queue.
 *      If an organ is found AND the patient is entitled due priority in having it,
 *          they go to transplant without addition in the waiting list and the organ is removed.
 * */
bool handleMatchingFromPatient(event_list *events, sim_time *t, BLOOD_TYPE bt, patient *patient) {

    PRIORITY pr = patient->priority, priority_placement = pr;
    BLOOD_TYPE avbOrganBt = bt;
    organ_bank *bank = &events->organ_arrival;
    organ_queue **organQueues = bank->queues;
    patient_queue_blood_type **btQueues = events->patient_arrival.blood_type_queues;
    bool found = false;

    // check if the patient can be transplanted due to their priority and other patients in queue
    if (pr == normal || pr == low) {
        if (btQueues[bt]->priority_queue[critical]->number > 0) {
            priority_placement = critical; // serve patient in queue critical
        } else if (pr == low && btQueues[bt]->priority_queue[normal]->number > 0) {
            priority_placement = normal; // serve patient in queue normal
        }
    }

#ifdef ABO_ID
    if (organQueues[bt]->number == 0) return found;
    found = true;
#else
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        if ((organQueues[i]->number != 0) && (COMPATIBLE(i, bt))) {
            avbOrganBt = i;
            found = true;
            break;
        }
    }
    if (!found) return found;
#endif

    if (priority_placement != pr) {
        // serve new selected patient and add original patient in queue
        addPatientToQueue(events, t, &events->patient_arrival.blood_type_queues[bt]->priority_queue[pr],
                          events->patient_arrival.blood_type_queues[bt], patient);
        patient = removePatient(0, &events->patient_arrival.blood_type_queues[bt]->priority_queue[priority_placement],
                                events->patient_arrival.blood_type_queues[bt], &events->patient_arrival,
                                t->current);
    }

    // get oldest organ
    organ *o = removeOrgan(0, &bank->queues[avbOrganBt], bank);
    addMatchedToTransplant(events, t, o, patient);

    return found;
}

/* Internal usage:
 *      - Upon the arrival of a new organ, this function search for a compatible patient in queue to allocate the organ to.
 *      - If the patient is found, the organ is transplanted without inserting it in the organ bank and the patient is removed.
 * */
bool checkCompatibility(BLOOD_TYPE bt, const BLOOD_TYPE *compatibles, int size_compatibles) {
    bool ret = false;

    for (int i = 0; i < size_compatibles; ++i) {
        if (bt == compatibles[i]) {
            ret = true;
            break;
        }
    }
    return ret;
}

void draftMatchingCompatible(){
    // TODO forse ti può servire? In realtà è praticamente equivalente a ABO id nella pratica!
    int i = 0;
    double prob = getMatchProb();
    while (i < NUM_PRIORITIES && !found) {
        for (int j = 0; j < size; ++j) {
            BLOOD_TYPE b = comp[j];
            switch (b) {
                case AB:
                    if (prob < MATCH_AB_P && (wl->blood_type_queues[b]->priority_queue[i]->number != 0)) {
                        avbPatientBt = AB;
                        avbPatientPr = i;
                        found = true;
                    }
                    break;
                case B:
                    if (prob < MATCH_B_P && (wl->blood_type_queues[b]->priority_queue[i]->number != 0)) {
                        avbPatientBt = B;
                        avbPatientPr = i;
                        found = true;
                    }
                    break;
                case A:
                    if (prob < MATCH_A_P && (wl->blood_type_queues[b]->priority_queue[i]->number != 0)) {
                        avbPatientBt = A;
                        avbPatientPr = i;
                        found = true;
                    }
                    break;
                case O:
                    if (prob < MATCH_O_P && (wl->blood_type_queues[b]->priority_queue[i]->number != 0)) {
                        avbPatientBt = O;
                        avbPatientPr = i;
                        found = true;
                    }
                    break;
                case nbt:
                    avbPatientBt = bt;
                    break;
            }
        }
        ++i;
    }

    if (!found) {
        // try checking identical ABO as last resort
        if (wl->blood_type_queues[bt]->number == 0) return found; // no patient in list for a ABO_identical transplant
        found = true;
        for (int x = 0; x < NUM_PRIORITIES; ++x) {
            if (wl->blood_type_queues[bt]->priority_queue[x]->number != 0) {
                avbPatientPr = x;
                avbPatientBt = bt;
                break;
            }
        }
    }
}