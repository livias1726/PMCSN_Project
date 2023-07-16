#include "headers/utils.h"

double getMinTime(event_list *events) {
    int len = 46;

    double timesToCompare[len];
    timesToCompare[0] = events->organ_arrival.interArrivalTime[O];
    timesToCompare[1] = events->organ_arrival.interArrivalTime[A];
    timesToCompare[2] = events->organ_arrival.interArrivalTime[B];
    timesToCompare[3] = events->organ_arrival.interArrivalTime[AB];
    timesToCompare[4] = events->patient_arrival.interArrivalTime[O][critical];
    timesToCompare[5] = events->patient_arrival.interArrivalTime[O][normal];
    timesToCompare[6] = events->patient_arrival.interArrivalTime[O][low];
    timesToCompare[7] = events->patient_arrival.interArrivalTime[A][critical];
    timesToCompare[8] = events->patient_arrival.interArrivalTime[A][normal];
    timesToCompare[9] = events->patient_arrival.interArrivalTime[A][low];
    timesToCompare[10] = events->patient_arrival.interArrivalTime[B][critical];
    timesToCompare[11] = events->patient_arrival.interArrivalTime[B][normal];
    timesToCompare[12] = events->patient_arrival.interArrivalTime[B][low];
    timesToCompare[13] = events->patient_arrival.interArrivalTime[AB][critical];
    timesToCompare[14] = events->patient_arrival.interArrivalTime[AB][normal];
    timesToCompare[15] = events->patient_arrival.interArrivalTime[AB][low];
    timesToCompare[16] = events->organs_loss.reneging_time[O];
    timesToCompare[17] = events->organs_loss.reneging_time[A];
    timesToCompare[18] = events->organs_loss.reneging_time[B];
    timesToCompare[19] = events->organs_loss.reneging_time[AB];
    timesToCompare[20] = events->patients_loss.reneging_time[O][critical];
    timesToCompare[21] = events->patients_loss.reneging_time[O][normal];
    timesToCompare[22] = events->patients_loss.reneging_time[O][low];
    timesToCompare[23] = events->patients_loss.reneging_time[A][critical];
    timesToCompare[24] = events->patients_loss.reneging_time[A][normal];
    timesToCompare[25] = events->patients_loss.reneging_time[A][low];
    timesToCompare[26] = events->patients_loss.reneging_time[B][critical];
    timesToCompare[27] = events->patients_loss.reneging_time[B][normal];
    timesToCompare[28] = events->patients_loss.reneging_time[B][low];
    timesToCompare[29] = events->patients_loss.reneging_time[AB][critical];
    timesToCompare[30] = events->patients_loss.reneging_time[AB][normal];
    timesToCompare[31] = events->patients_loss.reneging_time[AB][low];
    timesToCompare[32] = events->patients_loss.death_time[O][critical];
    timesToCompare[33] = events->patients_loss.death_time[O][normal];
    timesToCompare[34] = events->patients_loss.death_time[O][low];
    timesToCompare[35] = events->patients_loss.death_time[A][critical];
    timesToCompare[36] = events->patients_loss.death_time[A][normal];
    timesToCompare[37] = events->patients_loss.death_time[A][low];
    timesToCompare[38] = events->patients_loss.death_time[B][critical];
    timesToCompare[39] = events->patients_loss.death_time[B][normal];
    timesToCompare[40] = events->patients_loss.death_time[B][low];
    timesToCompare[41] = events->patients_loss.death_time[AB][critical];
    timesToCompare[42] = events->patients_loss.death_time[AB][normal];
    timesToCompare[43] = events->patients_loss.death_time[AB][low];
    timesToCompare[44] = getMinActivation(events->activation_arrival.inactive_patients);
    timesToCompare[45] = getMinTransplant(events->transplant_arrival.transplanted_patients);

    double min;
    GET_SMALLEST(timesToCompare, len, min)

    return min;
}

double getMinTransplant(in_transplant *transplanted) {
    double min, tmp;

    if (transplanted->next == NULL) return INFINITY;

    min = transplanted->next->completionTime;
    while (transplanted->next != NULL) {
        transplanted = transplanted->next;
        tmp = transplanted->completionTime;
        if (tmp < min) min = tmp;
    }
    return min;
}

double getMinActivation(in_activation *inactive) {
    double min, tmp;

    if (inactive->next == NULL) return INFINITY;

    min = inactive->next->completionTime;
    while (inactive->next != NULL) {
        inactive = inactive->next;
        tmp = inactive->completionTime;
        if (tmp < min) min = tmp;
    }
    return min;
}

void finiteSim(event_list *events, sim_time *t) {
    /* Choose next event selecting minimum time */
    t->current = 0;
    while (t->current < STOP) {
        t->next = getMinTime(events);		                //Next event time
        t->current = t->next;                               //Clock update

        for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
            if (t->current == events->organ_arrival.interArrivalTime[i]) {
                handleOrganArrival(events, t, i);
                break;
            } else if (t->current == events->organs_loss.reneging_time[i]) {
                handleOrganRenege(events, t, i);
                break;
            } else if (t->current == events->patient_arrival.interArrivalTime[i][critical]) {
                handlePatientArrival(events, t, i, critical);
                break;
            } else if (t->current == events->patient_arrival.interArrivalTime[i][normal]) {
                handlePatientArrival(events, t, i, normal);
                break;
            } else if (t->current == events->patient_arrival.interArrivalTime[i][low]) {
                handlePatientArrival(events, t, i, low);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][normal]) {
                handlePatientLoss(events, t, renege, i, normal);
                break;
            } else if (t->current == events->patients_loss.reneging_time[i][low]) {
                handlePatientLoss(events, t, renege, i, low);
                break;
            } else if (t->current == events->patients_loss.death_time[i][critical]) {
                handlePatientLoss(events, t, death, i, critical);
                break;
            } else if (t->current == events->patients_loss.death_time[i][normal]) {
                handlePatientLoss(events, t, death, i, normal);
                break;
            } else if (t->current == events->patients_loss.death_time[i][low]) {
                handlePatientLoss(events, t, death, i, low);
                break;
            } else if (t->current == getMinActivation(events->activation_arrival.inactive_patients)) {
                handlePatientActivation(events, t);
                break;
            } else if (t->current == getMinTransplant(events->transplant_arrival.transplanted_patients)) {
                handleTransplantCompletion(events, t);
                break;
            }
        }
    }
}
