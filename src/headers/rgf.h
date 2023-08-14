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

#ifdef ABO_ID
// Organ arrival rate [blood type][donor type]
#define LAMBDA_O (double[]){14.580822, 11.789041,  \
                            11.306849, 5.175342,    \
                            3.597260, 1.539726,    \
                            1.068493, 0.306849     }

// Patient arrival rate [blood type][priority]
#define LAMBDA_P (double[]){0.052055, 42.142466, 13.526027,  \
                            0.027397, 27.164384, 9.6684932,  \
                            0.027397, 12.846575, 3.9698630,  \
                            0.005479, 3.1780822, 1.1780822  }

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.002740, 2.394521, 3.460274,  \
                              0.000000, 1.008219, 1.906849,  \
                              0.000000, 0.739726, 1.046575,  \
                              0.000000, 0.093151, 0.180822  }

// Additional percentage of patients to renege from waiting list [blood type]
#define ADDITIONAL_EXIT_PERCENTAGE (double[]){0.46, /* O */ \
                                              0.55, /* A */ \
                                              0.53, /* B */ \
                                              0.68  /* AB */ }
// Patient renege rate [blood type][priority]
#define BASE_RENEGE_P (double[]){0.000000, 4.284932, 12.975342,  \
                                 0.005479, 2.367123, 7.315068,  \
                                 0.000000, 1.309589, 4.104110,   \
                                 0.000000, 0.238356, 0.632877   }

#define INC_RENEGE_P (double[]) {BASE_RENEGE_P[0]*ADDITIONAL_EXIT_PERCENTAGE[0], BASE_RENEGE_P[1]*ADDITIONAL_EXIT_PERCENTAGE[0], BASE_RENEGE_P[2]*ADDITIONAL_EXIT_PERCENTAGE[0], \
                                BASE_RENEGE_P[3]*ADDITIONAL_EXIT_PERCENTAGE[1], BASE_RENEGE_P[4]*ADDITIONAL_EXIT_PERCENTAGE[1], BASE_RENEGE_P[5]*ADDITIONAL_EXIT_PERCENTAGE[1], \
                                BASE_RENEGE_P[6]*ADDITIONAL_EXIT_PERCENTAGE[2], BASE_RENEGE_P[7]*ADDITIONAL_EXIT_PERCENTAGE[2], BASE_RENEGE_P[8]*ADDITIONAL_EXIT_PERCENTAGE[2], \
                                BASE_RENEGE_P[9]*ADDITIONAL_EXIT_PERCENTAGE[3], BASE_RENEGE_P[10]*ADDITIONAL_EXIT_PERCENTAGE[3], BASE_RENEGE_P[11]*ADDITIONAL_EXIT_PERCENTAGE[3] }

#define MU_RENEGE_P (double[]) {BASE_RENEGE_P[0]+INC_RENEGE_P[0], BASE_RENEGE_P[1]+INC_RENEGE_P[1], BASE_RENEGE_P[2]+INC_RENEGE_P[2], \
                                BASE_RENEGE_P[3]+INC_RENEGE_P[3], BASE_RENEGE_P[4]+INC_RENEGE_P[4], BASE_RENEGE_P[5]+INC_RENEGE_P[5], \
                                BASE_RENEGE_P[6]+INC_RENEGE_P[6], BASE_RENEGE_P[7]+INC_RENEGE_P[7], BASE_RENEGE_P[8]+INC_RENEGE_P[8], \
                                BASE_RENEGE_P[9]+INC_RENEGE_P[9], BASE_RENEGE_P[10]+INC_RENEGE_P[10], BASE_RENEGE_P[11]+INC_RENEGE_P[11] }

#else
// Organ arrival rate [blood type][donor type]
#define LAMBDA_O (double[]){11.124932, 10.253699,  \
                            8.634795, 4.456712,    \
                            2.777260, 1.334795,    \
                            0.803562, 0.206849     }

// Patient arrival rate [blood type][priority]
#define LAMBDA_P (double[]){0.049041, 35.550411, 13.090411,  \
                            0.027397, 23.596712, 9.120548,  \
                            0.013699, 10.779178, 4.012329,  \
                            0.004110, 2.717808, 1.093699       }

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.004110, 2.570685, 3.999178,  \
                              0.002740, 1.286301, 2.365205,  \
                              0.001370, 0.782466, 1.226849,  \
                              0.000274, 0.117534, 0.237260  }

// Patient renege rate [blood type][priority]
#define MU_RENEGE_P (double[]){0.006849, 3.943836, 11.293425,  \
                               0.003288, 2.243014, 6.688493,  \
                               0.001096, 1.180000, 3.483562,   \
                               0.000822, 0.208493, 0.662740   }

// Patient renege rate [blood type][priority]

/*#define MU_RENEGE_P (double[]){0.002, 2.677, 11.036, \
                               0.001, 1.387, 6.310,  \
                               0.001, 0.805, 3.441,  \
                               0.001, 0.129, 0.612   }*/

#endif


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

// Critical probability:
#define CRIT_P 0.3

//--------------------------------------------------------------------------------------------------------------------
double getOrganArrival(BLOOD_TYPE bt, DONOR_TYPE dt, double arrival);
double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getOrganRenege(double arrival);
double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getPatientDeath(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival);
double getActivationCompletion(double arrival);
double getTransplantCompletion(double arrival);
double getRejectionProb();
double getCriticalProb();
double getTransplantProb(BLOOD_TYPE bt);

#endif //PMCSN_PROJECT_RGF_H
