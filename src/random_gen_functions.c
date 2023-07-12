#include "headers/utils.h"
/***
* ------------------------------------------- RANDOM GENERATOR FUNCTIONS -----------------------------------------------
*/

double Exponential(double mu)
{
    return (-mu * log(1.0 - Random()));
}

double* getOrganArrival(BLOOD_TYPE bt, double arrival[NUM_BLOOD_TYPES]) {
    switch (bt) {
        case O:
            SelectStream(0);
            if (LAMBDA_ORG_O == 0)
                arrival[O] = (double) INFINITY;
            else
                arrival[O] += Exponential(1 / (LAMBDA_ORG_O));
            return arrival;
        case A:
            SelectStream(1);
            if (LAMBDA_ORG_A == 0)
                arrival[A] = (double) INFINITY;
            else
                arrival[A] += Exponential(1 / (LAMBDA_ORG_A));
            return arrival;
        case B:
            SelectStream(2);
            if (LAMBDA_ORG_B == 0)
                arrival[B] = (double) INFINITY;
            else
                arrival[B] += Exponential(1 / (LAMBDA_ORG_B));
            return arrival;
        case AB:
            SelectStream(3);
            if (LAMBDA_ORG_AB == 0)
                arrival[AB] = (double) INFINITY;
            else
                arrival[AB] += Exponential(1 / (LAMBDA_ORG_AB));
            return arrival;
        default:
            return arrival;
    }
}

double** getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, double arrival[4][3]) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(4);
                if (LAMBDA_PAT_O_CRIT == 0)
                    arrival[O][critical] = (double) INFINITY;
                else
                    arrival[O][critical] += Exponential(1/(LAMBDA_PAT_O_CRIT));
                return (double **) arrival;
            } else if (bt == A) {
                SelectStream(5);
                if (LAMBDA_PAT_A_CRIT == 0)
                    arrival[A][critical] = (double) INFINITY;
                else
                    arrival[A][critical] += Exponential(1/(LAMBDA_PAT_A_CRIT));
                return (double **) arrival;
            } else if (bt == B) {
                SelectStream(6);
                if (LAMBDA_PAT_B_CRIT == 0)
                    arrival[B][critical] = (double) INFINITY;
                else
                    arrival[B][critical] += Exponential(1/(LAMBDA_PAT_B_CRIT));
                return (double **) arrival;
            } else {
                SelectStream(7);
                if (LAMBDA_PAT_AB_CRIT == 0)
                    arrival[AB][critical] = (double) INFINITY;
                else
                    arrival[AB][critical] += Exponential(1/(LAMBDA_PAT_AB_CRIT));
                return (double **) arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(8);
                if (LAMBDA_PAT_O_NORM == 0)
                    arrival[O][normal] = (double) INFINITY;
                else
                    arrival[O][normal] += Exponential(1/(LAMBDA_PAT_O_NORM));
                return (double **) arrival;
            } else if (bt == A) {
                SelectStream(9);
                if (LAMBDA_PAT_A_NORM == 0)
                    arrival[A][normal] = (double) INFINITY;
                else
                    arrival[A][normal] += Exponential(1/(LAMBDA_PAT_A_NORM));
                return (double **) arrival;
            } else if (bt == B) {
                SelectStream(10);
                if (LAMBDA_PAT_B_NORM == 0)
                    arrival[B][normal] = (double) INFINITY;
                else
                    arrival[B][normal] += Exponential(1/(LAMBDA_PAT_B_NORM));
                return (double **) arrival;
            } else {
                SelectStream(11);
                if (LAMBDA_PAT_AB_NORM == 0)
                    arrival[AB][normal] = (double) INFINITY;
                else
                    arrival[AB][normal] += Exponential(1/(LAMBDA_PAT_AB_NORM));
                return (double **) arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(12);
                if (LAMBDA_PAT_O_LOW == 0)
                    arrival[O][low] = (double) INFINITY;
                else
                    arrival[O][low] += Exponential(1/(LAMBDA_PAT_O_LOW));
                return (double **) arrival;
            } else if (bt == A) {
                SelectStream(13);
                if (LAMBDA_PAT_A_LOW == 0)
                    arrival[A][low] = (double) INFINITY;
                else
                    arrival[A][low] += Exponential(1/(LAMBDA_PAT_A_LOW));
                return (double **) arrival;
            } else if (bt == B) {
                SelectStream(14);
                if (LAMBDA_PAT_B_LOW == 0)
                    arrival[B][low] = (double) INFINITY;
                else
                    arrival[B][low] += Exponential(1/(LAMBDA_PAT_B_LOW));
                return (double **) arrival;
            } else {
                SelectStream(15);
                if (LAMBDA_PAT_AB_LOW == 0)
                    arrival[AB][low] = (double) INFINITY;
                else
                    arrival[AB][low] += Exponential(1/(LAMBDA_PAT_AB_LOW));
                return (double **) arrival;
            }
        default:
            break;
    }
}

double getOrganRenege(BLOOD_TYPE bt, double arrival) {
    SelectStream(16);
    if (MU_ORG == 0)
        arrival = (double) INFINITY;
    else
        arrival += Exponential(1/(MU_ORG));
    return arrival;
}

double ** getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, double arrival[3][4]) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(17);
                if (MU_RENEGE_PAT_O_CRIT == 0)
                    arrival[O][critical] = (double) INFINITY;
                else
                    arrival[O][critical] += Exponential(1/(MU_RENEGE_PAT_O_CRIT));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(18);
                if (MU_RENEGE_PAT_A_CRIT == 0)
                    arrival[A][critical] = (double) INFINITY;
                else
                    arrival[A][critical] += Exponential(1/(MU_RENEGE_PAT_A_CRIT));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(19);
                if (MU_RENEGE_PAT_B_CRIT == 0)
                    arrival[B][critical] = (double) INFINITY;
                else
                    arrival[B][critical] += Exponential(1/(MU_RENEGE_PAT_B_CRIT));
                return (double**) arrival;
            } else {
                SelectStream(20);
                if (MU_RENEGE_PAT_AB_CRIT == 0)
                    arrival[AB][critical] = (double) INFINITY;
                else
                    arrival[AB][critical] += Exponential(1/(MU_RENEGE_PAT_AB_CRIT));
                return (double**) arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(21);
                if (MU_RENEGE_PAT_O_NORM == 0)
                    arrival[O][normal] = (double) INFINITY;
                else
                    arrival[O][normal] += Exponential(1/(MU_RENEGE_PAT_O_NORM));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(22);
                if (MU_RENEGE_PAT_A_NORM == 0)
                    arrival[A][normal] = (double) INFINITY;
                else
                    arrival[A][normal] += Exponential(1/(MU_RENEGE_PAT_A_NORM));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(23);
                if (MU_RENEGE_PAT_B_NORM == 0)
                    arrival[B][normal] = (double) INFINITY;
                else
                    arrival[B][normal] += Exponential(1/(MU_RENEGE_PAT_B_NORM));
                return (double**) arrival;
            } else {
                SelectStream(24);
                if (MU_RENEGE_PAT_AB_NORM == 0)
                    arrival[AB][normal] = (double) INFINITY;
                else
                    arrival[AB][normal] += Exponential(1/(MU_RENEGE_PAT_AB_NORM));
                return (double**) arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(25);
                if (MU_RENEGE_PAT_O_LOW == 0)
                    arrival[O][low] = (double) INFINITY;
                else
                    arrival[O][low] += Exponential(1/(MU_RENEGE_PAT_O_LOW));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(26);
                if (MU_RENEGE_PAT_A_LOW == 0)
                    arrival[A][low] = (double) INFINITY;
                else
                    arrival[A][low] += Exponential(1/(MU_RENEGE_PAT_A_LOW));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(27);
                if (MU_RENEGE_PAT_B_LOW == 0)
                    arrival[B][low] = (double) INFINITY;
                else
                    arrival[B][low] += Exponential(1/(MU_RENEGE_PAT_B_LOW));
                return (double**) arrival;
            } else {
                SelectStream(28);
                if (MU_RENEGE_PAT_AB_LOW == 0)
                    arrival[AB][low] = (double) INFINITY;
                else
                    arrival[AB][low] += Exponential(1/(MU_RENEGE_PAT_AB_LOW));
                return (double**) arrival;
            }
        default:
            break;
    }
}

double ** getPatientDeath(PRIORITY pr, BLOOD_TYPE bt, double arrival[3][4]) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(29);
                if (MU_DEATH_PAT_O_CRIT == 0)
                    arrival[O][critical] = (double) INFINITY;
                else
                    arrival[O][critical] += Exponential(1/(MU_DEATH_PAT_O_CRIT));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(30);
                if (MU_DEATH_PAT_A_CRIT == 0)
                    arrival[A][critical] = (double) INFINITY;
                else
                    arrival[A][critical] += Exponential(1/(MU_DEATH_PAT_A_CRIT));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(31);
                if (MU_DEATH_PAT_B_CRIT == 0)
                    arrival[B][critical] = (double) INFINITY;
                else
                    arrival[B][critical] += Exponential(1/(MU_DEATH_PAT_B_CRIT));
                return (double**) arrival;
            } else {
                SelectStream(32);
                if (MU_DEATH_PAT_AB_CRIT == 0)
                    arrival[AB][critical] = (double) INFINITY;
                else
                    arrival[AB][critical] += Exponential(1/(MU_DEATH_PAT_AB_CRIT));
                return (double**) arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(33);
                if (MU_DEATH_PAT_O_NORM == 0)
                    arrival[O][normal] = (double) INFINITY;
                else
                    arrival[O][normal] += Exponential(1/(MU_DEATH_PAT_O_NORM));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(34);
                if (MU_DEATH_PAT_A_NORM == 0)
                    arrival[A][normal] = (double) INFINITY;
                else
                    arrival[A][normal] += Exponential(1/(MU_DEATH_PAT_A_NORM));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(35);
                if (MU_DEATH_PAT_B_NORM == 0)
                    arrival[B][normal] = (double) INFINITY;
                else
                    arrival[B][normal] += Exponential(1/(MU_DEATH_PAT_B_NORM));
                return (double**) arrival;
            } else {
                SelectStream(36);
                if (MU_DEATH_PAT_AB_NORM == 0)
                    arrival[AB][normal] = (double) INFINITY;
                else
                    arrival[AB][normal] += Exponential(1/(MU_DEATH_PAT_AB_NORM));
                return (double**) arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(37);
                if (MU_DEATH_PAT_O_LOW == 0)
                    arrival[O][low] = (double) INFINITY;
                else
                    arrival[O][low] += Exponential(1/(MU_DEATH_PAT_O_LOW));
                return (double**) arrival;
            } else if (bt == A) {
                SelectStream(38);
                if (MU_DEATH_PAT_A_LOW == 0)
                    arrival[A][low] = (double) INFINITY;
                else
                    arrival[A][low] += Exponential(1/(MU_DEATH_PAT_A_LOW));
                return (double**) arrival;
            } else if (bt == B) {
                SelectStream(39);
                if (MU_DEATH_PAT_B_LOW == 0)
                    arrival[B][low] = (double) INFINITY;
                else
                    arrival[B][low] += Exponential(1/(MU_DEATH_PAT_B_LOW));
                return (double**) arrival;
            } else {
                SelectStream(40);
                if (MU_DEATH_PAT_AB_LOW == 0)
                    arrival[AB][low] = (double) INFINITY;
                else
                    arrival[AB][low] += Exponential(1/(MU_DEATH_PAT_AB_LOW));
                return (double**) arrival;
            }
        default:
            break;
    }
}