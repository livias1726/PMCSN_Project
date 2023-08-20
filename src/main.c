#include "headers/utils.h"
#include "headers/sim.h"
#include "headers/stats.h"
#include "headers/init.h"

void resetWaitingList(patient_waiting_list *wl) {
    patient *head, *node;
    int i,j;

    wl->total_number = 0.0;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            wl->num_completions[i][j] = 0.0;
            wl->num_arrivals[i][j] = 0.0;

            head = wl->blood_type_queues[i]->priority_queue[j]->queue;
            node = head->next;
            RESET_QUEUE(head, node)
            wl->blood_type_queues[i]->priority_queue[j]->number = 0;
        }

        wl->blood_type_queues[i]->number = 0;
    }
}

void resetOrganBank(organ_bank *bank) {
    organ *head, *node;
    int i,j;
    bank->total_number = 0.0;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < 2; ++j) {
            bank->num_completions[i][j] = 0.0;
            bank->num_arrivals[i][j] = 0.0;
        }

        head = bank->queues[i]->queue;
        node = head->next;
        RESET_QUEUE(head, node)
        bank->queues[i]->number = 0;
    }
}

void resetTransplantCenter(transplant_center *transplant) {
    in_transplant *head, *node;
    int i,j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            transplant->num_arrivals[i][j] = 0.0;
            transplant->num_completions[i][j] = 0.0;
            transplant->num_rejections[i][j] = 0.0;
        }
    }

    transplant->total_number = 0.0;
    transplant->min_transplant = INFINITY;

    head = transplant->transplanted_patients;
    node = head->next;
    RESET_QUEUE_2(head, node, matched)
}

void resetActivationCenter(activation_center *activation) {
    in_activation *head, *node;

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        activation->num_completions[i] = 0.0;
        activation->num_arrivals[i] = 0.0;

        head = activation->inactive_patients[i]->head;
        node = head->next;
        RESET_QUEUE_2(head, node, patient)

        activation->inactive_patients[i]->number = 0;
        activation->inactive_patients[i]->min_activation = INFINITY;
    }

    activation->total_number = 0.0;
    activation->min_activation = INFINITY;
}

void resetOrgansExpiredQueue(organs_expired *organs_loss) {
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organs_loss->num_renege[i] = 0.0;
        organs_loss->reneging_time[i] = -1;
    }
}

void resetPatientLostQueue(patients_lost *patients_loss) {
    int i,j;
    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            patients_loss->number_dead[i][j] = 0.0;
            patients_loss->death_time[i][j] = -1;
            patients_loss->number_renege[i][j] = 0.0;
            patients_loss->reneging_time[i][j] = -1;
        }
    }
}

void resetTimeStatistics(time_integrated_stats *ti_stats){
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        ti_stats->area_bank[i]->node = 0;
        ti_stats->area_bank[i]->queue = 0;
        ti_stats->area_bank[i]->service = 0;
        for (j = 0; j < NUM_PRIORITIES; ++j) {
            ti_stats->area_waiting_list[i][j]->node = 0;
            ti_stats->area_waiting_list[i][j]->queue = 0;
            ti_stats->area_waiting_list[i][j]->service = 0;
        }
    }

    ti_stats->area_activation->node = 0;
    ti_stats->area_activation->queue = 0;
    ti_stats->area_activation->service = 0;

    ti_stats->area_transplant->node = 0;
    ti_stats->area_transplant->queue = 0;
    ti_stats->area_transplant->service = 0;
}

void resetSimulation(event_list *events, sim_time *sim_time, time_integrated_stats *ti_stats) {

    resetWaitingList(&events->patient_arrival);
    resetOrganBank(&events->organ_arrival);
    resetTransplantCenter(&events->transplant_arrival);
    resetActivationCenter(&events->activation_arrival);
    resetPatientLostQueue(&events->patients_loss);
    resetOrgansExpiredQueue(&events->organs_loss);

    initializeEventTime(events);

    *sim_time = initializeTime();

    resetTimeStatistics(ti_stats);
}

int main(){

    char *model, *policy;
    event_list *events;
    sim_time sim_time;
    time_integrated_stats *ti_stats;
    int i, num_iter;
    stats **batches = NULL, *final_stat, *mid_stats[ITERATIONS];
    time_t s, e;

    // ---------------------------------------------- Intro --------------------------------------------------------

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

    printf("Simulation: Finite horizon - %s model (Policy: ABO %s)\n\n", model, policy);

    // --------------------------------------------- Initialization --------------------------------------------------

    events = initializeEventList();
    initializeEventTime(events);

    sim_time = initializeTime();
    ti_stats = initializeTimeStatistics();

    // batches for each observation year
    num_iter = (STOP / BATCH_SIZE)*60 + 1; // measure for each month
    batches = malloc(num_iter * sizeof(stats*));
    MALLOC_HANDLER(batches)
    for (i = 0; i < num_iter; ++i) {
        batches[i] = initializeStatistics();
    }

    // --------------------------------------------- Simulation ------------------------------------------------------

    for (i = 0; i < ITERATIONS; ++i) {
        mid_stats[i] = initializeStatistics();

        PlantSeeds(SEED);

        s = clock();
        infiniteSim(events, &sim_time, ti_stats, batches, mid_stats[i], &num_iter);
        e = clock();
        printf("Simulation %d - time: %lld\n", i, (e-s)/CLOCKS_PER_SEC);

        computeFinalStatistics(mid_stats[i], batches, num_iter);
        saveResultsCsv(i, mid_stats[i], false, 0);
        //saveResultsLean(mid_stat);

        resetSimulation(events, &sim_time, ti_stats);
    }

    // ----------------------------------------------------- Results --------------------------------------------------

    final_stat = initializeStatistics();
    computeFinalStatistics(final_stat, mid_stats, ITERATIONS);

    saveResultsCsv(i, final_stat, false, 0);
    //saveResultsLean(final_stat);

    // ----------------------------------------------- Clean up -----------------------------------------------------

    cleanUpEventList(events);
    cleanUpTimeStatistics(ti_stats);
    for (i = 0; i < num_iter; ++i) {
        cleanUpStatistics(batches[i]);
    }
    cleanUpStatistics(final_stat);
}
