
#ifndef PMCSN_PROJECT_ENUMS_H
#define PMCSN_PROJECT_ENUMS_H

#endif //PMCSN_PROJECT_ENUMS_H

// ---------------------------------------- MODEL ENUMS ---------------------------------------------------
typedef enum blood_type {
    O,
    A,
    B,
    AB
} BLOOD_TYPE;

typedef enum priority {
    critical,
    normal,
    low
} PRIORITY;

typedef enum policy {
    ABO_Id,
    ABO_Comp
} POLICY;

// ------------------------------- STATE ---------------------------
// These states are important to determine the allocation policy

typedef enum organ_status {
    AVAILABLE,
    UNAVAILABLE
} ORGAN_STATUS;

typedef enum patient_queue_status {
    EMPTY,
    NOT_EMPTY
} PATIENT_A_CRIT_STATUS, PATIENT_B_CRIT_STATUS, PATIENT_AB_CRIT_STATUS, PATIENT_0_CRIT_STATUS,
        PATIENT_A_NORM_STATUS, PATIENT_B_NORM_STATUS, PATIENT_AB_NORM_STATUS, PATIENT_0_NORM_STATUS,
        PATIENT_A_LOW_STATUS, PATIENT_B_LOW_STATUS, PATIENT_AB_LOW_STATUS, PATIENT_0_LOW_STATUS;