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
    timesToCompare[16] = events->organsLoss.renegingTime[O];
    timesToCompare[17] = events->organsLoss.renegingTime[A];
    timesToCompare[18] = events->organsLoss.renegingTime[B];
    timesToCompare[19] = events->organsLoss.renegingTime[AB];
    timesToCompare[20] = events->patientsLoss.renegingTime[O][critical];
    timesToCompare[21] = events->patientsLoss.renegingTime[O][normal];
    timesToCompare[22] = events->patientsLoss.renegingTime[O][low];
    timesToCompare[23] = events->patientsLoss.renegingTime[A][critical];
    timesToCompare[24] = events->patientsLoss.renegingTime[A][normal];
    timesToCompare[25] = events->patientsLoss.renegingTime[A][low];
    timesToCompare[26] = events->patientsLoss.renegingTime[B][critical];
    timesToCompare[27] = events->patientsLoss.renegingTime[B][normal];
    timesToCompare[28] = events->patientsLoss.renegingTime[B][low];
    timesToCompare[29] = events->patientsLoss.renegingTime[AB][critical];
    timesToCompare[30] = events->patientsLoss.renegingTime[AB][normal];
    timesToCompare[31] = events->patientsLoss.renegingTime[AB][low];
    timesToCompare[32] = events->patientsLoss.deathTime[O][critical];
    timesToCompare[33] = events->patientsLoss.deathTime[O][normal];
    timesToCompare[34] = events->patientsLoss.deathTime[O][low];
    timesToCompare[35] = events->patientsLoss.deathTime[A][critical];
    timesToCompare[36] = events->patientsLoss.deathTime[A][normal];
    timesToCompare[37] = events->patientsLoss.deathTime[A][low];
    timesToCompare[38] = events->patientsLoss.deathTime[B][critical];
    timesToCompare[39] = events->patientsLoss.deathTime[B][normal];
    timesToCompare[40] = events->patientsLoss.deathTime[B][low];
    timesToCompare[41] = events->patientsLoss.deathTime[AB][critical];
    timesToCompare[42] = events->patientsLoss.deathTime[AB][normal];
    timesToCompare[43] = events->patientsLoss.deathTime[AB][low];
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

void finite_sim(event_list *events, sim_time *t, stats *statistics) {
    /* Choose next event selecting minimum time */
    t->current = 0;
    while (t->current < STOP) {
        t->next = getMinTime(events);		                //Next event time
        t->current = t->next;                             //Clock update

        for (int i = 0; i < NUM_BLOOD_TYPES; ++i) {
            if (t->current == events->organArrival.interArrivalTime[i]) {
                handleOrganArrival(events, t, i);
                statistics->numOrganArrivals[i]++;
                break;
            } else if (t->current == events->organsLoss.renegingTime[i]) {
                handleOrganRenege(events, t, i);
                statistics->numOrganOutdatings[i]++;
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][critical]) {
                handlePatientArrival(events, t, i, critical);
                statistics->numPatientArrivals[i*NUM_PRIORITIES + critical]++;
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][normal]) {
                handlePatientArrival(events, t, i, normal);
                statistics->numPatientArrivals[i*NUM_PRIORITIES + normal]++;
                break;
            } else if (t->current == events->patientArrival.interArrivalTime[i][low]) {
                handlePatientArrival(events, t, i, low);
                statistics->numPatientArrivals[i*NUM_PRIORITIES + low]++;
                break;
            } else if (t->current == events->patientsLoss.renegingTime[i][critical]) {
                handlePatientLoss(events, t, renege, i, critical);
                statistics->numReneges[i*NUM_PRIORITIES + critical]++;
                break;
            } else if (t->current == events->patientsLoss.renegingTime[i][normal]) {
                handlePatientLoss(events, t, renege, i, normal);
                statistics->numReneges[i*NUM_PRIORITIES + normal]++;
                break;
            } else if (t->current == events->patientsLoss.renegingTime[i][low]) {
                handlePatientLoss(events, t, renege, i, low);
                statistics->numReneges[i*NUM_PRIORITIES + low]++;
                break;
            } else if (t->current == events->patientsLoss.deathTime[i][critical]) {
                handlePatientLoss(events, t, death, i, critical);
                statistics->numDeaths[i*NUM_PRIORITIES + critical]++;
                break;
            } else if (t->current == events->patientsLoss.deathTime[i][normal]) {
                handlePatientLoss(events, t, death, i, normal);
                statistics->numDeaths[i*NUM_PRIORITIES + normal]++;
                break;
            } else if (t->current == events->patientsLoss.deathTime[i][low]) {
                handlePatientLoss(events, t, death, i, low);
                statistics->numDeaths[i*NUM_PRIORITIES + low]++;
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
