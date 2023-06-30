
#define NUM_PATIENT_QUEUES 12
#define NUM_ORGAN_QUEUES 4
#define START 0.0                      /* initial time                   */
#define STOP 0.0                       /* terminal (close the door) time */
#define INFINITY   (100.0 * STOP)      /* must be much larger than STOP  */

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

typedef struct activation_delay {
    double number_in_delay;
} delay;

typedef struct transplant {
    double completed_transplant;
    double repeated_transplant;
} transplant;

typedef struct simulation_event {
    char* name;
    double arrival;
    double completion;
} event;

typedef struct simulation_time {
    event current_event;
    double current;
    double next;
    double last;
} t;


// ------------------------------- STATE ---------------------------
// These states are important to determine the allocation policy

typedef enum organ_status {
    AVAILABLE,
    UNAVAILABLE
} ORGAN_A_STATUS, ORGAN_B_STATUS, ORGAN_AB_STATUS, ORGAN_0_STATUS;

typedef enum patient_queue_status {
    EMPTY,
    NOT_EMPTY
} PATIENT_A_CRIT_STATUS, PATIENT_B_CRIT_STATUS, PATIENT_AB_CRIT_STATUS, PATIENT_0_CRIT_STATUS,
    PATIENT_A_NORM_STATUS, PATIENT_B_NORM_STATUS, PATIENT_AB_NORM_STATUS, PATIENT_0_NORM_STATUS,
    PATIENT_A_LOW_STATUS, PATIENT_B_LOW_STATUS, PATIENT_AB_LOW_STATUS, PATIENT_0_LOW_STATUS;