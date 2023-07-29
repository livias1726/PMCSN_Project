#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

// Patient arrival rate [blood type][priority]
#define LAMBDA_P (double[]){0.052, 37.68, 12.233, 0.029, 24.666, 8.642, 0.016, 11.501, 3.725, 0.005, 2.880, 1.019}

// Organ arrival rate [blood type][donor type]
#define LAMBDA_O (double[]){12.11, 10.31, 9.40, 4.54, 2.99, 1.34, 0.87, 0.22}

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.003, 2.504, 3.947, 0.001, 1.221, 2.324, 0.000, 0.776, 1.212, 0.000, 0.105, 0.225}

// Patient death rate [blood type][priority]
#define MU_RENEGE_P (double[]){0.006, 4.168, 13.043, 0.001, 2.338, 7.623, 0.000, 1.244, 3.999, 0.001, 0.211, 0.756}

// Organ expire rate (1 day)
#define MU_ORG 1.0

#ifdef ABO_ID
#define REJECT_P 0.0
#else
#define REJECT_P 0.12
#endif

#define MIN_TRANSPLANT 12.0
#define MAX_TRANSPLANT 24.0             // 12-24 giorni di trapianto + monitoraggio

// Transplant probability : P(Bt AND Pr)
#define TRANSPLANT_PROB (double[]){0.0004, 0.4147, 0.0333, 0.0002, 0.3280, 0.0317, 0.0002, 0.1257, 0.0100, 0.00004, 0.0452, 0.0033}

double getOrganArrival(BLOOD_TYPE bt, DONOR_TYPE dt, double arrival);
double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getOrganRenege(double arrival);
double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getPatientDeath(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getTransplantCompletion(double arrival);
double getRejectionProb();
double getTransplantProb(BLOOD_TYPE bt, PRIORITY pr);

#endif //PMCSN_PROJECT_RGF_H
