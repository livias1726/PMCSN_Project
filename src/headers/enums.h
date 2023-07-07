#ifndef PMCSN_PROJECT_ENUMS_H
#define PMCSN_PROJECT_ENUMS_H

// ---------------------------------------- MODEL ENUMS ---------------------------------------------------
typedef enum blood_type {
    O,
    A,
    B,
    AB
} BLOOD_TYPE;

static const char * const bt_to_str[] = {
    [O] = "0",
    [A] = "A",
    [B] = "B",
    [AB] = "AB"
};

typedef enum priority {
    none,
    critical=0,
    normal=1,
    low=2
} PRIORITY;

static const char * const prio_to_str[] = {
    [critical] = "critical",
    [normal] = "normal",
    [low] = "low",
};

typedef enum policy {
    ABO_Id,
    ABO_Comp
} POLICY;

typedef enum loss_reason {
    death,
    renege
} LOSS_REASON;

#endif //PMCSN_PROJECT_ENUMS_H