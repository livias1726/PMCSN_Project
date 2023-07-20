#include "headers/utils.h"
/***
* ------------------------------------------- RANDOM GENERATOR FUNCTIONS -----------------------------------------------
*/

double TruncatedNormal(double a, double b) {
    /* approximate mean and stdev from max and min */
    double mu = (a+b)/2;
    double s = (b-a)/4;
    double alpha = (a-mu)/s;
    double beta = (b-mu)/s;

    /* calculate pdf and cdf of a standard normal from alpha and beta */
    double pdf = pdfNormal(0,1, beta) - pdfNormal(0, 1, alpha);
    double cdf = cdfNormal(0,1,beta)-cdfNormal(0,1,alpha);

    double t_mean = mu - s*(pdf)/(cdf);
    double t_var = pow(s, 2) * (1 - (beta* pdfNormal(0,1,beta)-alpha* pdfNormal(0,1,alpha))/(cdf) - (pow(pdf/cdf, 2)));
    return Normal(t_mean, sqrt(t_var));
}

double getDecDonorOrganArrival(BLOOD_TYPE bt, double arrival) {
    switch (bt) {
        case O:
            SelectStream(0);
            if (LAMBDA_ORG_DEC_O == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_DEC_O));
            return arrival;
        case A:
            SelectStream(1);
            if (LAMBDA_ORG_DEC_A == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_DEC_A));
            return arrival;
        case B:
            SelectStream(2);
            if (LAMBDA_ORG_DEC_B == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_DEC_B));
            return arrival;
        case AB:
            SelectStream(3);
            if (LAMBDA_ORG_DEC_AB == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_DEC_AB));
            return arrival;
        default:
            return arrival;
    }
}

double getLivingDonorOrganArrival(BLOOD_TYPE bt, double arrival) {
    switch (bt) {
        case O:
            SelectStream(44);
            if (LAMBDA_ORG_LIVING_O == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_LIVING_O));
            return arrival;
        case A:
            SelectStream(45);
            if (LAMBDA_ORG_LIVING_A == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_LIVING_A));
            return arrival;
        case B:
            SelectStream(46);
            if (LAMBDA_ORG_LIVING_B == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_LIVING_B));
            return arrival;
        case AB:
            SelectStream(47);
            if (LAMBDA_ORG_LIVING_AB == 0)
                arrival = (double) INFINITY;
            else
                arrival += Exponential(1 / (LAMBDA_ORG_LIVING_AB));
            return arrival;
        default:
            return arrival;
    }
}

double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, double arrival) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(4);
                if (LAMBDA_PAT_O_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_O_CRIT));
                return arrival;
            } else if (bt == A) {
                SelectStream(5);
                if (LAMBDA_PAT_A_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_A_CRIT));
                return arrival;
            } else if (bt == B) {
                SelectStream(6);
                if (LAMBDA_PAT_B_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_B_CRIT));
                return arrival;
            } else {
                SelectStream(7);
                if (LAMBDA_PAT_AB_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_AB_CRIT));
                return arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(8);
                if (LAMBDA_PAT_O_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_O_NORM));
                return arrival;
            } else if (bt == A) {
                SelectStream(9);
                if (LAMBDA_PAT_A_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_A_NORM));
                return arrival;
            } else if (bt == B) {
                SelectStream(10);
                if (LAMBDA_PAT_B_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_B_NORM));
                return arrival;
            } else {
                SelectStream(11);
                if (LAMBDA_PAT_AB_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_AB_NORM));
                return arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(12);
                if (LAMBDA_PAT_O_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_O_LOW));
                return arrival;
            } else if (bt == A) {
                SelectStream(13);
                if (LAMBDA_PAT_A_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_A_LOW));
                return arrival;
            } else if (bt == B) {
                SelectStream(14);
                if (LAMBDA_PAT_B_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_B_LOW));
                return arrival;
            } else {
                SelectStream(15);
                if (LAMBDA_PAT_AB_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(LAMBDA_PAT_AB_LOW));
                return arrival;
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

double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, double arrival) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(17);
                if (MU_RENEGE_PAT_O_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_O_CRIT));
                return arrival;
            } else if (bt == A) {
                SelectStream(18);
                if (MU_RENEGE_PAT_A_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_A_CRIT));
                return arrival;
            } else if (bt == B) {
                SelectStream(19);
                if (MU_RENEGE_PAT_B_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_B_CRIT));
                return arrival;
            } else {
                SelectStream(20);
                if (MU_RENEGE_PAT_AB_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_AB_CRIT));
                return arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(21);
                if (MU_RENEGE_PAT_O_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_O_NORM));
                return arrival;
            } else if (bt == A) {
                SelectStream(22);
                if (MU_RENEGE_PAT_A_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_A_NORM));
                return arrival;
            } else if (bt == B) {
                SelectStream(23);
                if (MU_RENEGE_PAT_B_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_B_NORM));
                return arrival;
            } else {
                SelectStream(24);
                if (MU_RENEGE_PAT_AB_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_AB_NORM));
                return arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(25);
                if (MU_RENEGE_PAT_O_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_O_LOW));
                return arrival;
            } else if (bt == A) {
                SelectStream(26);
                if (MU_RENEGE_PAT_A_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_A_LOW));
                return arrival;
            } else if (bt == B) {
                SelectStream(27);
                if (MU_RENEGE_PAT_B_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_B_LOW));
                return arrival;
            } else {
                SelectStream(28);
                if (MU_RENEGE_PAT_AB_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_RENEGE_PAT_AB_LOW));
                return arrival;
            }
        default:
            break;
    }
}

double getPatientDeath(PRIORITY pr, BLOOD_TYPE bt, double arrival) {
    switch (pr) {
        case critical:
            if (bt == O) {
                SelectStream(29);
                if (MU_DEATH_PAT_O_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_O_CRIT));
                return arrival;
            } else if (bt == A) {
                SelectStream(30);
                if (MU_DEATH_PAT_A_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_A_CRIT));
                return arrival;
            } else if (bt == B) {
                SelectStream(31);
                if (MU_DEATH_PAT_B_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_B_CRIT));
                return arrival;
            } else {
                SelectStream(32);
                if (MU_DEATH_PAT_AB_CRIT == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_AB_CRIT));
                return arrival;
            }
        case normal:
            if (bt == O) {
                SelectStream(33);
                if (MU_DEATH_PAT_O_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_O_NORM));
                return arrival;
            } else if (bt == A) {
                SelectStream(34);
                if (MU_DEATH_PAT_A_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_A_NORM));
                return arrival;
            } else if (bt == B) {
                SelectStream(35);
                if (MU_DEATH_PAT_B_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_B_NORM));
                return arrival;
            } else {
                SelectStream(36);
                if (MU_DEATH_PAT_AB_NORM == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_AB_NORM));
                return arrival;
            }
        case low:
            if (bt == O) {
                SelectStream(37);
                if (MU_DEATH_PAT_O_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_O_LOW));
                return arrival;
            } else if (bt == A) {
                SelectStream(38);
                if (MU_DEATH_PAT_A_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_A_LOW));
                return arrival;
            } else if (bt == B) {
                SelectStream(39);
                if (MU_DEATH_PAT_B_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_B_LOW));
                return arrival;
            } else {
                SelectStream(40);
                if (MU_DEATH_PAT_AB_LOW == 0)
                    arrival = (double) INFINITY;
                else
                    arrival += Exponential(1/(MU_DEATH_PAT_AB_LOW));
                return arrival;
            }
        default:
            break;
    }
}

double getRejectionProb() {
    SelectStream(41);
    return Random();
}

double getActivationCompletion(double arrival) {
    SelectStream(42);
    arrival += TruncatedNormal(MIN_ACTIVATION, MAX_ACTIVATION);
    return arrival;
}

double getTransplantCompletion(double arrival) {
    SelectStream(43);
    arrival += TruncatedNormal(MIN_TRANSPLANT, MAX_TRANSPLANT);
    return arrival;
}