#ifndef PMCSN_PROJECT_RGF_H
#define PMCSN_PROJECT_RGF_H

#define LAMBDA_PAT_O_CRIT 0.053
#define LAMBDA_PAT_O_NORM 33.904
#define LAMBDA_PAT_O_LOW 13.547
#define LAMBDA_PAT_A_CRIT 0.027
#define LAMBDA_PAT_A_NORM 22.676
#define LAMBDA_PAT_A_LOW 9.327
#define LAMBDA_PAT_B_CRIT 0.013
#define LAMBDA_PAT_B_NORM 10.222
#define LAMBDA_PAT_B_LOW 4.189
#define LAMBDA_PAT_AB_CRIT 0.004
#define LAMBDA_PAT_AB_NORM 2.599
#define LAMBDA_PAT_AB_LOW 1.084

#define LAMBDA_ORG_O 10.350
#define LAMBDA_ORG_A 8.037
#define LAMBDA_ORG_B 2.579
#define LAMBDA_ORG_AB 0.736

#define MU_DEATH_PAT_O_CRIT 0.003
#define MU_DEATH_PAT_O_NORM 2.626
#define MU_DEATH_PAT_O_LOW 4.157
#define MU_DEATH_PAT_A_CRIT 0.004
#define MU_DEATH_PAT_A_NORM 1.353
#define MU_DEATH_PAT_A_LOW 2.517
#define MU_DEATH_PAT_B_CRIT 0.001
#define MU_DEATH_PAT_B_NORM 0.789
#define MU_DEATH_PAT_B_LOW 1.288
#define MU_DEATH_PAT_AB_CRIT 0.000
#define MU_DEATH_PAT_AB_NORM 0.122
#define MU_DEATH_PAT_AB_LOW 0.254

#define MU_RENEGE_PAT_O_CRIT 0.011
#define MU_RENEGE_PAT_O_NORM 8.666
#define MU_RENEGE_PAT_O_LOW 12.336
#define MU_RENEGE_PAT_A_CRIT 0.005
#define MU_RENEGE_PAT_A_NORM 6.392
#define MU_RENEGE_PAT_A_LOW 7.977
#define MU_RENEGE_PAT_B_CRIT 0.001
#define MU_RENEGE_PAT_B_NORM 2.642
#define MU_RENEGE_PAT_B_LOW 3.756
#define MU_RENEGE_PAT_AB_CRIT 0.001
#define MU_RENEGE_PAT_AB_NORM 0.645
#define MU_RENEGE_PAT_AB_LOW 0.826

#define MU_ORG 1

#define REJECT_P 0.11

#define MIN_ACTIVATION 730.0
#define MAX_ACTIVATION 1095.0           // 2*365-3*365 giorni di attesa
#define MEAN_ACTIVATION 912.5
#define S_ACTIVATION 182.5

#define MIN_TRANSPLANT 12.0
#define MAX_TRANSPLANT 24.0             // 12-24 giorni di trapianto + monitoraggio

double getOrganArrival(BLOOD_TYPE bt, double arrival);
double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getOrganRenege(BLOOD_TYPE bt, double arrival);
double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getPatientDeath(PRIORITY pr, BLOOD_TYPE bt, double arrival);
double getActivationCompletion(double arrival);
double getTransplantCompletion(double arrival);
double getRejectionProb();
#endif //PMCSN_PROJECT_RGF_H
