
#define NUM_PATIENT_QUEUES 12
#define NUM_ORGAN_QUEUES 4

// ---------------------------------------- MODEL ELEMENTS ---------------------------------------------------
typedef enum blood_type {
    O,
    A,
    B,
    AB
} blood_type;

typedef enum priority {
    critical,
    normal,
    low
} priority;

typedef enum policy {
    ABO_Id,
    ABO_Comp
} POLICY;

typedef struct patient_queue {
    blood_type bt;
    priority urg;
    double numArrivals;
    double numDeaths;
    double numRenege;
    double numTransplants;
} patient_queue;

typedef struct organ_queue {
    blood_type bt;
    double numArrivals;
    double numOutdatings;
    double numTransplants;
} organ_queue;
