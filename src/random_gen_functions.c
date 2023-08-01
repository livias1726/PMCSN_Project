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

double getOrganArrival(BLOOD_TYPE bt, DONOR_TYPE dt, double arrival) {
    int x = VALUE(bt, dt, 2), stream = 0;

    SelectStream(stream + x);
    if (LAMBDA_O[x] == 0) {
        arrival = (double) INFINITY;
    }else{
        arrival += Exponential(1 / LAMBDA_O[x]);
    }

    return arrival;
}

double getPatientArrival(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival) {
    int x = (pt == inactive) ? VALUE(bt, 2, 3) : VALUE(bt, pr, 3);
    int stream = 8;

    SelectStream(stream + x);
    if (LAMBDA_P[x] == 0) {
        arrival = (double) INFINITY;
    }else{
        arrival += Exponential(1 / LAMBDA_P[x]);
    }

    return arrival;
}

// streams [20]
double getOrganRenege(double arrival) {
    SelectStream(20);

    if (!MU_ORG) {
        arrival = (double) INFINITY;
    }else {
        arrival += Exponential(1 / MU_ORG);
    }

    return arrival;
}

double getPatientRenege(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival) {
    int x = (pt == inactive) ? VALUE(bt, 2, 3) : VALUE(bt, pr, 3);
    int stream = 21;

    SelectStream(stream + x);
    if (MU_RENEGE_P[x] == 0) {
        arrival = (double) INFINITY;
    }else{
        arrival += Exponential(1 / MU_RENEGE_P[x]);
    }

    return arrival;
}

double getPatientDeath(BLOOD_TYPE bt, PRIORITY pr, PATIENT_TYPE pt, double arrival) {
    int x = (pt == inactive) ? VALUE(bt, 2, 3) : VALUE(bt, pr, 3);
    int stream = 33;

    SelectStream(stream + x);
    if (MU_DEATH_P[x] == 0) {
        arrival = (double) INFINITY;
    }else{
        arrival += Exponential(1 / MU_DEATH_P[x]);
    }

    return arrival;
}

double getRejectionProb() {
    SelectStream(45);
    return Random();
}

double getActivationCompletion(double arrival) {
    SelectStream(46);
    arrival += TruncatedNormal(MIN_ACTIVATION, MAX_ACTIVATION);
    return arrival;
}

double getTransplantCompletion(double arrival) {
    SelectStream(47);
    arrival += TruncatedNormal(MIN_TRANSPLANT, MAX_TRANSPLANT);
    return arrival;
}

double getTransplantProb(BLOOD_TYPE bt) {
    int x = bt, stream = 48;

    SelectStream(stream + x);

    return Random();
}