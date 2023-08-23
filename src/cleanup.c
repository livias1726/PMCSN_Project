#include "headers/cleanup.h"

static void resetWaitingList(patient_waiting_list *wl) {
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

static void resetOrganBank(organ_bank *bank) {
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

static void resetTransplantCenter(transplant_center *transplant) {
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

static void resetActivationCenter(activation_center *activation) {
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

static void resetOrgansExpiredQueue(organs_expired *organs_loss) {
    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        organs_loss->num_renege[i] = 0.0;
        organs_loss->reneging_time[i] = -1;
    }
}

static void resetPatientLostQueue(patients_lost *patients_loss) {
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

static void resetTimeStatistics(time_integrated_stats *ti_stats){
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

static void cleanUpOrganBank(organ_bank *bank) {
    organ *current; // head
    organ *next; // first node

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        current = bank->queues[i]->queue; // head
        next = current->next; // first node

        RESET_QUEUE(current, next)

        free(bank->queues[i]->queue);
        free(bank->queues[i]);
    }
}

static void cleanUpWaitingList(patient_waiting_list *wt_list) {
    patient *current, *next;
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        for (j = 0; j < NUM_PRIORITIES; ++j) {

            current = wt_list->blood_type_queues[i]->priority_queue[j]->queue;  // head
            next = current->next;   // first

            //RESET_QUEUE(current, next)

            free(wt_list->blood_type_queues[i]->priority_queue[j]->queue);
            free(wt_list->blood_type_queues[i]->priority_queue[j]);
        }
        free(wt_list->blood_type_queues[i]);
    }
}

static void cleanUpActivationCenter(activation_center *act_center) {
    in_activation *current, *next; // first node

    for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
        current = act_center->inactive_patients[i]->head; // head
        next = current->next; // first node

        RESET_QUEUE_2(current, next, patient)

        free(act_center->inactive_patients[i]->head);
        free(act_center->inactive_patients[i]);
    }
}

static void cleanUpTransplantCenter(transplant_center *trans_center) {
    in_transplant *current = trans_center->transplanted_patients; // head
    in_transplant *next = current->next; // first node

    RESET_QUEUE_2(current, next, matched)

    free(trans_center->transplanted_patients); // free head
}

void cleanUpEventList(event_list* events){

    cleanUpOrganBank(&events->organ_arrival);
    cleanUpWaitingList(&events->patient_arrival);
    cleanUpActivationCenter(&events->activation_arrival);
    cleanUpTransplantCenter(&events->transplant_arrival);

    free(events);
}

void cleanUpTimeStatistics(time_integrated_stats *ti_stats) {
    int i,j;

    for (i = 0; i < NUM_BLOOD_TYPES; ++i) {
        free(ti_stats->area_bank[i]);

        for (j = 0; j < NUM_PRIORITIES; ++j) {
            free(ti_stats->area_waiting_list[i][j]);
        }
    }

    free(ti_stats->area_activation);
    free(ti_stats->area_transplant);
    free(ti_stats);
}

void cleanUpStatistics(stats *statistics){

    free(statistics->wl_stats);
    free(statistics->ob_stats);
    free(statistics->act_stats);
    free(statistics->trans_stats);

    free(statistics);
}