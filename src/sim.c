#include "headers/utils.h"

double getMinTime(event_list *events) {
    int len = 46;

    double timesToCompare[len];
    timesToCompare[0] = events->organArrival.interArrivalTime[O];
    timesToCompare[1] = events->organArrival.interArrivalTime[A];
    timesToCompare[2] = events->organArrival.interArrivalTime[B];
    timesToCompare[3] = events->organArrival.interArrivalTime[AB];
    timesToCompare[4] = events->patientArrival.interArrivalTime[O][critical];
    timesToCompare[5] = events->patientArrival.interArrivalTime[O][normal];
    timesToCompare[6] = events->patientArrival.interArrivalTime[O][low];
    timesToCompare[7] = events->patientArrival.interArrivalTime[A][critical];
    timesToCompare[8] = events->patientArrival.interArrivalTime[A][normal];
    timesToCompare[9] = events->patientArrival.interArrivalTime[A][low];
    timesToCompare[10] = events->patientArrival.interArrivalTime[B][critical];
    timesToCompare[11] = events->patientArrival.interArrivalTime[B][normal];
    timesToCompare[12] = events->patientArrival.interArrivalTime[B][low];
    timesToCompare[13] = events->patientArrival.interArrivalTime[AB][critical];
    timesToCompare[14] = events->patientArrival.interArrivalTime[AB][normal];
    timesToCompare[15] = events->patientArrival.interArrivalTime[AB][low];
    timesToCompare[16] = events->organsLoss.reneging_time[O];
    timesToCompare[17] = events->organsLoss.reneging_time[A];
    timesToCompare[18] = events->organsLoss.reneging_time[B];
    timesToCompare[19] = events->organsLoss.reneging_time[AB];
    timesToCompare[20] = events->patientsLoss.reneging_time[O][critical];
    timesToCompare[21] = events->patientsLoss.reneging_time[O][normal];
    timesToCompare[22] = events->patientsLoss.reneging_time[O][low];
    timesToCompare[23] = events->patientsLoss.reneging_time[A][critical];
    timesToCompare[24] = events->patientsLoss.reneging_time[A][normal];
    timesToCompare[25] = events->patientsLoss.reneging_time[A][low];
    timesToCompare[26] = events->patientsLoss.reneging_time[B][critical];
    timesToCompare[27] = events->patientsLoss.reneging_time[B][normal];
    timesToCompare[28] = events->patientsLoss.reneging_time[B][low];
    timesToCompare[29] = events->patientsLoss.reneging_time[AB][critical];
    timesToCompare[30] = events->patientsLoss.reneging_time[AB][normal];
    timesToCompare[31] = events->patientsLoss.reneging_time[AB][low];
    timesToCompare[32] = events->patientsLoss.death_time[O][critical];
    timesToCompare[33] = events->patientsLoss.death_time[O][normal];
    timesToCompare[34] = events->patientsLoss.death_time[O][low];
    timesToCompare[35] = events->patientsLoss.death_time[A][critical];
    timesToCompare[36] = events->patientsLoss.death_time[A][normal];
    timesToCompare[37] = events->patientsLoss.death_time[A][low];
    timesToCompare[38] = events->patientsLoss.death_time[B][critical];
    timesToCompare[39] = events->patientsLoss.death_time[B][normal];
    timesToCompare[40] = events->patientsLoss.death_time[B][low];
    timesToCompare[41] = events->patientsLoss.death_time[AB][critical];
    timesToCompare[42] = events->patientsLoss.death_time[AB][normal];
    timesToCompare[43] = events->patientsLoss.death_time[AB][low];
    timesToCompare[44] = getMinActivation(events->activationArrival.inactive_patients);
    timesToCompare[45] = getMinTransplant(events->transplantArrival.transplanted_patients);

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

void finite_sim(event_list *events, sim_time *t) {
    /* Choose next event selecting minimum time */
    t->current = 0;
    while (t->current < STOP) {
        t->next = getMinTime(events);		                //Next event time
        t->current = t->next;                               //Clock update

        for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
            if (t->current == events->organArrival.interArrivalTime[i]) {
                handleOrganArrival(events, t, i);
                break;
            } else if (t->current == events->organsLoss.reneging_time[i]) {
                handleOrganRenege(events, t, i);
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][critical]) {
                handlePatientArrival(events, t, i, critical);
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][normal]) {
                handlePatientArrival(events, t, i, normal);
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][low]) {
                handlePatientArrival(events, t, i, low);
                break;
            } else if (t->current == events->patientsLoss.reneging_time[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical);
                break;
            } else if (t->current == events->patientsLoss.reneging_time[i][normal]) {
                handlePatientLoss(events, t, renege, i, normal);
                break;
            } else if (t->current == events->patientsLoss.reneging_time[i][low]) {
                handlePatientLoss(events, t, renege, i, low);
                break;
            } else if (t->current == events->patientsLoss.death_time[i][critical]) {
                handlePatientLoss(events, t, death, i, critical);
                break;
            } else if (t->current == events->patientsLoss.death_time[i][normal]) {
                handlePatientLoss(events, t, death, i, normal);
                break;
            } else if (t->current == events->patientsLoss.death_time[i][low]) {
                handlePatientLoss(events, t, death, i, low);
                break;
            } else if (t->current == getMinActivation(events->activationArrival.inactive_patients)) {
                handlePatientActivation(events, t);
                break;
            } else if (t->current == getMinTransplant(events->transplantArrival.transplanted_patients)) {
                handleTransplantCompletion(events, t);
                break;
            }
        }
    }
}
