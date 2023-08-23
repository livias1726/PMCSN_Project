#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"
#include "model.h"

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
#define LAMBDA_P (double[]){0.040548, 31.715890, 11.494247, \
                            0.021370, 20.740822, 7.908219,  \
                            0.012329, 9.636164, 3.526849,   \
                            0.003836, 2.372877, 0.943014 }

// Patient death rate [blood type][priority]
#define MU_DEATH_P (double[]){0.004110, 2.570411, 3.998904,  \
                              0.002740, 1.286301, 2.365205,  \
                              0.001370, 0.782466, 1.226849,  \
                              0.000274, 0.117534, 0.236986  }

// Patient renege rate [blood type][priority]
#define MU_RENEGE_P (double[]){0.006849, 3.943836, 11.293425, \
                                0.003288, 2.243014, 6.688493,  \
                                0.001096, 1.180000, 3.483562,  \
                                0.000822, 0.208493, 0.662740   }
#endif


// Organ expire rate (1 day)
#define MU_ORG 1.0

#ifdef IMPROVEMENT
#define REJECT_P (double [4][2]) {(0.13 + 0.0637), (0.13 + 0.0637),\
                                (0.13 + 0.0637), (0.13 + 0.0637), \
                                (0.13 + 0.0637), (0.13 + 0.0637),\
                                (0.13 + 0.0637), (0.13 + 0.0637)}

#else
    #ifdef ABO_ID
        #define REJECT_P (double []) {0.0, /* O */ \
                                      0.0, /* A */\
                                      0.0, /* B */\
                                      0.0  /* AB */}
    #else
        #define REJECT_P (double [4][2]) {{0.13,  0.13}, \
                                          {0.13,  0.13}, \
                                          {0.13,  0.13}, \
                                          {0.13,  0.13}}
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
#define TRANSPLANT_PROB (double[]){0.5, \
                                    0.5,\
                                    0.5,\
                                    0.5 }

// Critical probability:
#define CRIT_INACT_P 0.0

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
