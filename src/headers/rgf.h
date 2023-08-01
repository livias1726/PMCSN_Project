#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"

// Patient arrival rate [blood type][priority]
#define LAMBDA_P (double[]){0.052, 37.68, 12.233, /* O */ \
                            0.029, 24.666, 8.642, /* A */ \
                            0.016, 11.501, 3.725, /* B */ \
                            0.005, 2.880, 1.019 /* AB */ }

// Organ arrival rate [blood type][donor type]
#define LAMBDA_O (double[]){12.11, 10.31, /* O */ \
                            9.40, 4.54,   /* A */ \
                            2.99, 1.34,   /* B */ \
                            0.87, 0.22    /* AB */ }

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.003, 2.504, 3.947, /* O */ \
                              0.001, 1.221, 2.324, /* A */ \
                              0.000, 0.776, 1.212, /* B */ \
                              0.000, 0.105, 0.225 /* AB */ }

// Patient renege rate [blood type][priority]
#define MU_RENEGE_P (double[]){0.006, 4.168, 13.043, /* O */ \
                               0.001, 2.338, 7.623, /* A */ \
                               0.000, 1.244, 3.999,  /* B */ \
                               0.001, 0.211, 0.756 /* AB */ }

// Organ expire rate (1 day)
#define MU_ORG 1.0

#ifdef IMPROVEMENT
#define REJECT_P (0.13 + 0.0637)
#else
    #ifdef ABO_ID
        #define REJECT_P 0.0
    #else
        #define REJECT_P 0.13
    #endif
#endif

// 1/2 anni di attesa
#define MIN_ACTIVATION 730.0
#define MAX_ACTIVATION 1095.0

#ifdef IMPROVEMENT
#define MIN_TRANSPLANT 46.0
#define MAX_TRANSPLANT 58.0             // 12-24 giorni di trapianto + monitoraggio
#else
#define MIN_TRANSPLANT 12.0
#define MAX_TRANSPLANT 24.0  // 12-24 giorni di trapianto + monitoraggio
#endif

// Transplant probability : P(Bt AND Pr)
#define TRANSPLANT_PROB (double[]){0.45, /* O */ \
                                   0.36, /* A */ \
                                   0.14, /* B */ \
                                   0.05 /* AB */ }

//--------------------------------------------------------------------------------------------------------------------
double getOrganArrival(BLOOD_TYPE bt, DONOR_TYPE dt, double arrival);
double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getOrganRenege(double arrival);
double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getPatientDeath(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getActivationCompletion(double arrival);
double getTransplantCompletion(double arrival);
double getRejectionProb();
double getTransplantProb(BLOOD_TYPE bt);

#endif //PMCSN_PROJECT_RGF_H
