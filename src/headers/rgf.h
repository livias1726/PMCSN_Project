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

#define MU_DEATH_PAT_O_CRIT 0.013
#define MU_DEATH_PAT_O_NORM 3.227
#define MU_DEATH_PAT_O_LOW 1.137
#define MU_DEATH_PAT_A_CRIT 0.009
#define MU_DEATH_PAT_A_NORM 1.703
#define MU_DEATH_PAT_A_LOW 0.679
#define MU_DEATH_PAT_B_CRIT 0.005
#define MU_DEATH_PAT_B_NORM 1.032
#define MU_DEATH_PAT_B_LOW 0.352
#define MU_DEATH_PAT_AB_CRIT 0.001
#define MU_DEATH_PAT_AB_NORM 0.174
#define MU_DEATH_PAT_AB_LOW 0.061

#define MU_RENEGE_PAT_O_CRIT 0.026
#define MU_RENEGE_PAT_O_NORM 6.754
#define MU_RENEGE_PAT_O_LOW 1.971
#define MU_RENEGE_PAT_A_CRIT 0.018
#define MU_RENEGE_PAT_A_NORM 4.707
#define MU_RENEGE_PAT_A_LOW 1.257
#define MU_RENEGE_PAT_B_CRIT 0.007
#define MU_RENEGE_PAT_B_NORM 2.029
#define MU_RENEGE_PAT_B_LOW 0.605
#define MU_RENEGE_PAT_AB_CRIT 0.001
#define MU_RENEGE_PAT_AB_NORM 0.443
#define MU_RENEGE_PAT_AB_LOW 0.106

#define MU_ORG 1

double Exponential(double mu);
double getOrganArrival(BLOOD_TYPE bt, double arrival);
double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getOrganRenege(BLOOD_TYPE bt, double arrival);
double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, double arrival);
double getPatientDeath(PRIORITY pr, BLOOD_TYPE bt, double arrival);
#endif //PMCSN_PROJECT_RGF_H
