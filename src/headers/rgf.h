#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

#include "../../lib/rngs.h"
#include "../../lib/rvms.h"
#include "../../lib/rvgs.h"
#include "model.h"

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

// Organ expire rate (1 day)
#define MU_ORG 1.0

#ifdef IMPROVEMENT
#define REJECT_P (double []) {0.13, 0.49}
#else
    #ifdef ABO_ID
        #define REJECT_P 0.0
    #else
        #define REJECT_P 0.13
    #endif
#endif

// 2/3 anni di attesa
#define MIN_ACTIVATION 0
#define MAX_ACTIVATION (6 * 365)

#ifdef IMPROVEMENT
#define MIN_TRANSPLANT 46.0
#define MAX_TRANSPLANT 58.0             // 12-24 giorni di trapianto + monitoraggio
#else
#define MIN_TRANSPLANT 12.0
#define MAX_TRANSPLANT 24.0  // 12-24 giorni di trapianto + monitoraggio
#endif

// Transplant probability : P(Bt AND Pr)
#define TRANSPLANT_PROB (double[]){0.54, /* O */ \
                                   0.62, /* A */ \
                                   0.63, /* B */ \
                                   0.67 /* AB */ }

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
