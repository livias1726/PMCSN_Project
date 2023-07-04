#include "headers/utils.h"

#define START 0.0                      /* initial time                   */
#define STOP 2190                      /* terminal (close the door) time - 365*6 */
#define INFINITE   (100.0 * STOP)      /* must be much larger than STOP  */

double simulation_time = START;

void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank* bank);
void handleOrganRenege(BLOOD_TYPE bloodType, organ_bank *pBank);
void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list* list);

void addOrganToQueue(organ_queue **pQueue, BLOOD_TYPE bloodType);
void addPatientToQueue(patient_queue **pQueue, BLOOD_TYPE type, PRIORITY priority);
void removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue);

// Functions to initialize structs where to recover statistics for each center of the model
patient_waiting_list initialize_waiting_list() {
    patient_waiting_list waitingList;
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        waitingList.queues[i] = NULL;
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
    handleOrganArrival(O, &bank);
    handleOrganArrival(A, &bank);
    handleOrganArrival(O, &bank);

    /* new patient arrival */
    handlePatientArrival(O, normal, &waiting_list);
    handlePatientArrival(O, low, &waiting_list);
    handlePatientArrival(O, normal, &waiting_list);
    handlePatientArrival(O, critical, &waiting_list);
    handlePatientArrival(O, normal, &waiting_list);
    handlePatientArrival(O, low, &waiting_list);
    handlePatientArrival(O, low, &waiting_list);

    /* patient death */

    /* patient renege */

    /* organ renege */
    //handleOrganRenege(O, &bank);

    /*FIXME clean_up(sizeof(organ_bank), (void **) &bank);
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
        free(waiting_list.queues[i]);
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

void handleOrganRenege(enum blood_type bloodType, organ_bank *bank) {
    bank->total_number = bank->total_number - 1;
    switch (bloodType) {
        case O:
            removeExpiredOrgans(bloodType, &bank->queues[0]);
            break;
        case A:
            removeExpiredOrgans(bloodType, &bank->queues[1]);
            break;
        case B:
            removeExpiredOrgans(bloodType, &bank->queues[2]);
            break;
        case AB:
            removeExpiredOrgans(bloodType, &bank->queues[3]);
            break;
        default:
            break;
    }
}

void removeExpiredOrgans(BLOOD_TYPE bloodType, organ_queue **pQueue) {
    organ *o_queue = (*pQueue)->queue;
    organ *current;

    while (o_queue->next != NULL) {
        current = o_queue->next;
        if (current->starting_age > 24) {
            /* The organ has expired */

        }
    }
}

void handlePatientArrival(BLOOD_TYPE bloodType, PRIORITY priority, patient_waiting_list *list) {
    list->total_number = list->total_number + 1;
    switch (bloodType) {
        case O:
            addPatientToQueue(&list->queues[0], bloodType, priority);
            malloc_handler(1, (void *[]) {list->queues[0]});
            printf("Arrived patient with blood type 0\n");
            break;
        case A:
            addPatientToQueue(&list->queues[1], bloodType, priority);
            //FIXME malloc_handler(sizeof(patient), (void **) &list.queues[1]);
            printf("Arrived patient with blood type A\n");
            break;
        case B:
            addPatientToQueue(&list->queues[2], bloodType, priority);
            //FIXME malloc_handler(sizeof(patient), (void **) &list.queues[2]);
            printf("Arrived patient with blood type B\n");
            break;
        case AB:
            addPatientToQueue(&list->queues[3], bloodType, priority);
            //FIXME malloc_handler(sizeof(patient), (void **) &list.queues[3]);
            printf("Arrived organ with blood type AB\n");
            break;
        default:
            break;
    }
}

void handleOrganArrival(BLOOD_TYPE bloodType, organ_bank *bank) {
    bank->total_number = bank->total_number + 1;
    switch (bloodType) {
        case O:
            addOrganToQueue(&bank->queues[0], O);
            malloc_handler(1, (void *[]) {bank->queues[0]});
            printf("Arrived organ with blood type 0\n");
            break;
        case A:
            addOrganToQueue(&bank->queues[1], A);
            //FIXME malloc_handler(sizeof(organ), (void **) &bank.queues[1]);
            printf("Arrived organ with blood type A\n");
            break;
        case B:
            addOrganToQueue(&bank->queues[2], B);
            //FIXME malloc_handler(sizeof(organ), (void **) &bank.queues[2]);
            printf("Arrived organ with blood type B\n");
            break;
        case AB:
            addOrganToQueue(&bank->queues[3], AB);
            //FIXME malloc_handler(sizeof(organ), (void **) &bank.queues[3]);
            printf("Arrived organ with blood type AB\n");
            break;
        default:
            break;
    }
}

void addOrganToQueue(organ_queue **pQueue, BLOOD_TYPE bloodType) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(organ_queue));
    }

    /* Set queue attributes */
    (*pQueue)->bt = bloodType;
    (*pQueue)->number = (*pQueue)->number+1;
    (*pQueue)->organ_available = true;

    /* Check if queue is empty */
    if ((*pQueue)->queue == NULL) {
        printf("pointer to queue is NULL: no organs in queue\n");
        (*pQueue)->queue = malloc(sizeof(organ));
        (*pQueue)->queue[0].starting_age = (rand() % (24-0+1)) + 0; /* Starting age is between 0 and 24 h *///FIXME use Random() instead of rand()
        return;
    }

    /* Add organ to list (not empty) */
    printf("pointer to queue is NOT NULL: there are organs in queue\n");
    organ *o_queue = (*pQueue)->queue;
    while (o_queue->next != NULL) {
        o_queue = o_queue->next;
    }
    o_queue->next = malloc(sizeof(organ));
    o_queue->next->starting_age = (rand() % (24-0+1)) + 0; //FIXME use Random() instead of rand()
}

void addPatientToQueue(patient_queue **pQueue, BLOOD_TYPE type, PRIORITY priority) {
    if ((*pQueue) == NULL) {
        (*pQueue) = malloc(sizeof(patient_queue));
    }

    if ((*pQueue)->queue == NULL) {
        printf("pointer to queue is NULL: no patients in queue\n");
        /* set list head */
        (*pQueue)->queue = malloc(sizeof(patient));
        (*pQueue)->queue[0].priority = none;

        /* add new patient */
        patient *new = malloc(sizeof(patient));
        new->prev = (*pQueue)->queue;
        (*pQueue)->queue->next = new;
        (*pQueue)->queue->next->priority = priority;
        (*pQueue)->queue->next->is_active = rand() % 2; //FIXME integrate with probability to be inactive and handle it
        return;
    }

    printf("pointer to queue is NOT NULL: there are patients in queue\n");
    patient *p_queue = (*pQueue)->queue;

    /* Add patient to list - ordering by priority */
    patient *new = malloc(sizeof(patient));
    new->priority = priority;
    new->is_active = rand() % 2; //FIXME integrate with probability to be inactive and handle it

    printf("%d", new->priority);
    printf("%d", p_queue->priority);

    patient *current = p_queue;
    patient *prev = NULL;

    while (current->priority > new->priority) {
        // Next in the list
        if (current->next != NULL) {
            current = current->next;
            prev = current->prev;
        } else {
            current->next = new;
            new->prev = current;
            return;
        }
    }

    prev->next = new;
    current->prev = new;
    new->prev = prev;
    new->next = current;
}