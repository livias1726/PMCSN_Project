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
    critical,
    normal,
    low
} PRIORITY;

typedef enum policy {
    ABO_Id,
    ABO_Comp
} POLICY;

#endif //PMCSN_PROJECT_ENUMS_H