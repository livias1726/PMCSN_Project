#ifndef PMCSN_PROJECT_MODEL_H
#define PMCSN_PROJECT_MODEL_H

#define NUM_BLOOD_TYPES 4
#define NUM_PRIORITIES 3

// ---------------------------------------- MODEL ENUMS ---------------------------------------------------
typedef enum blood_type {
    O,
    A,
    B,
    AB,
    nbt
} BLOOD_TYPE;

static const char * const bt_to_str[] = {
        [O] = "0",
        [A] = "A",
        [B] = "B",
        [AB] = "AB"
};

typedef enum priority {
    critical=0,
    normal,
    low,
    none
} PRIORITY;

static const char * const pr_to_str[] = {
        [critical] = "critical",
        [normal] = "normal",
        [low] = "low",
};

typedef enum loss_reason {
    death,
    renege
} LOSS_REASON;

// --------------------------------------------------- MODEL USERS ---------------------------------------------
typedef struct patient {
    BLOOD_TYPE bt;          /* patient blood type */
    PRIORITY priority;      /* needed to order patients in list based on the priority */
    bool is_active;         /* the patient is active (true or false) */
    time_t start_time;      /* time of waiting list addition */
    time_t end_time;        /* time of waiting list removal */
    struct patient *next;   /* pointer to the next patient in queue */
} patient;

typedef struct organ {
    BLOOD_TYPE bt;          /* organ blood type */
    double starting_age;    /* this is a random number that models the fact that the organ is moved in different states */
    struct organ *next;
} organ;

typedef struct matched {    /* this is a struct that represents the organs that were matched with a patient */
    organ organ;
    patient patient;
    struct matched *next;
} matched;

typedef struct in_activation {
    patient* patient;
    double serverOffset;           /* offset to the server */
    double completionTime;      /* time of activation completion - t_{c,del} */
    struct in_activation *next;
} in_activation;

typedef struct in_transplant {
    matched* matched;           /* list of the organs matched with patients */
    double serverOffset;           /* offset to the server */
    double completionTime;      /* time of the transplant completion - t_{c,trans} */
    struct in_transplant *next;
} in_transplant;

// -------------------------------------- CENTERS STRUCTS ----------------------------------------------------
typedef struct patient_queue_priority {
    PRIORITY priority;
    patient *queue;
    double number;
} patient_queue_priority;

typedef struct patient_queue_blood_type {
    BLOOD_TYPE bt;
    patient_queue_priority *priority_queue[NUM_PRIORITIES];             /* head of the patient priority queues */
    bool patient_available;                                             /* true = available/not empty, false = unavailable/empty - x_{p,BT} */
    double number;                                                      /* l_{p,BT} */
} patient_queue_blood_type;

typedef struct patient_waiting_list {
    patient_queue_blood_type *blood_type_queues[NUM_BLOOD_TYPES];
    double total_number;                                                /* l_{p} */
    double interArrivalTime[NUM_BLOOD_TYPES][NUM_PRIORITIES];           /* t_{a,p,BT} */
    double numPatientArrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
} patient_waiting_list;

typedef struct organ_queue {
    BLOOD_TYPE bt;
    organ* queue;               /* head of the organ queue */
    bool organ_available;       /* true = available, false = unavailable - x_{o,BT} - SPECIFICATION */
    double number;              /* l_{o,BT} */
} organ_queue;

typedef struct organ_bank {
    organ_queue *queues[NUM_BLOOD_TYPES];              /* organ queues for blood type */
    double total_number;                                /* l_{o} */
    double interArrivalTime[NUM_BLOOD_TYPES];           /* t_{a,o,BT} */
    double numOrganArrivals[NUM_BLOOD_TYPES];
} organ_bank;

typedef struct transplant_center {
    in_transplant *transplanted_patients;
    double total_number;                                /* l_{trans} */
    double completed_transplants;
    double rejected_transplants;
} transplant;

typedef struct activation_center {
    in_activation *inactive_patients;               /* list of the inactive patients */
    double total_number;                            /* l_{del} */
} activation;

/* loss queues */
typedef struct patient_lost_queue {
    patient *queue;
    double number_dead[NUM_BLOOD_TYPES][NUM_PRIORITIES];                /* number dead type bt and priority pr - l_{d,p,BT} */
    double number_renege[NUM_BLOOD_TYPES][NUM_PRIORITIES];              /* number reneging type bt and priority pr - l_{r,p,BT} */
    double reneging_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];               /* t_{r,p,BT} */
    double death_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];                  /* t_{d,p,BT} */
} patients_lost;

typedef struct organs_expired_queue {
    organ *queue;
    double number[NUM_BLOOD_TYPES];                     /* number expired type bt */
    double reneging_time[NUM_BLOOD_TYPES];               /* t_{r,o,BT} */
} organs_expired;

#endif