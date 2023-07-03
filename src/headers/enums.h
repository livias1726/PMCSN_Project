#ifndef PMCSN_PROJECT_ENUMS_H
#define PMCSN_PROJECT_ENUMS_H

// ---------------------------------------- MODEL ENUMS ---------------------------------------------------
typedef enum blood_type {
    O,
    A,
    B,
    AB
} BLOOD_TYPE;

typedef enum priority {
    critical=1,
    normal=2,
    low=3,
    none
} PRIORITY;

typedef enum policy {
    ABO_Id,
    ABO_Comp
} POLICY;

#endif //PMCSN_PROJECT_ENUMS_H