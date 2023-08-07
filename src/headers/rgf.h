#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"

/* TODO: test tassi incrementali
static double LAMBDA_P[] = {0.044, 32.348, 13.784,
                            0.033, 22.268, 9.203,
                            0.014, 9.636, 4.159,
                            0.003, 2.507, 1.252};
static double lambda_p_inc[] = {0.092, 0.028, 0.006};

static double LAMBDA_O[] = {9.45, 10.91,
                            7.23, 4.63,
                            2.45, 1.50,
                            0.71,0.16};
static double lambda_o_inc[] = {0.05, 0.011};

static double MU_DEATH_P[] = {0.011, 2.792, 3.962,
                            0.005, 1.433, 2.293,
                            0.008, 0.888, 1.156,
                            0.003, 0.170, 0.293};
static double mu_dp_dec[] = {0.295, 0.023, 0.016};

static double MU_RENEGE_P[] = {0.011, 3.564, 7.553,
                               0.008, 1.964, 4.504,
                               0.005, 1.038, 2.315,
                               0.000, 0.203, 0.370};
static double mu_rp_inc[] = {0.083, 0.023, 0.063};
*/

/**/
// Patient arrival rate [blood type][priority]
#define LAMBDA_P (double[]){0.052, 37.68, 12.233,  \
                            0.029, 24.666, 8.642,  \
                            0.016, 11.501, 3.725,  \
                            0.005, 2.880, 1.019  }

// Organ arrival rate [blood type][donor type]
#define LAMBDA_O (double[]){12.11, 10.31,  \
                            9.40, 4.54,    \
                            2.99, 1.34,    \
                            0.87, 0.22     }

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.003, 2.504, 3.947,  \
                              0.001, 1.221, 2.324,  \
                              0.000, 0.776, 1.212,  \
                              0.000, 0.105, 0.225  }

// Patient renege rate [blood type][priority]
#define MU_RENEGE_P (double[]){0.002, 2.677, 11.036, \
                               0.001, 1.387, 6.310,  \
                               0.001, 0.805, 3.441,  \
                               0.001, 0.129, 0.612   }

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

// 2/3 anni di attesa
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
