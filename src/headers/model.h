#ifndef PMCSN_PROJECT_MODEL_H
#define PMCSN_PROJECT_MODEL_H

#define NUM_BLOOD_TYPES 4
#define NUM_PRIORITIES 3

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
    critical=0,
    active,
    inactive,
    none
} PRIORITY;

static const char * const pr_to_str[] = {
    [critical] = "critical",
    [active] = "active",
    [inactive] = "inactive",
};

typedef enum loss_reason {
    death,
    renege
} LOSS_REASON;

typedef enum donor_type{
    deceased,
    living
} DONOR_TYPE;

/** --------------------------------------------------- MODEL USERS ---------------------------------------------------
 *  The system must handle 2 types of 'user':
 *      - Patient
 *      - Organ
 *  These can eventually be a part of a third pseudo-user after a match.
 */

/**
 * PATIENT: a user that is added to the system because in need of a transplant due to health conditions.
 *      Each patient is characterized by:
 *      - A blood type, that determines the matching with an organ according to the allocation policy in use.
 *      - A priority, which depends on the clinical urgency required by the patient, that determines the order of service.
 *      - An active flag, that determines the actual availability of the patient to receive an organ.
 *      - The time of addition in waiting list according to the simulation time.
 * */
typedef struct patient {
    BLOOD_TYPE bt;          /* patient blood type */
    PRIORITY priority;      /* needed to order patients in list based on the priority */
    double start_time;      /* time of waiting list addition */
    struct patient *next;   /* pointer to the next patient in queue */
} patient;

/**
 * ORGAN: a user that is added to the system to serve the requests for a transplant from patients.
 *      Each organ is characterized by:
 *      - A blood type, that determines the matching with a patient according to the allocation policy in use.
 * */
typedef struct organ {
    BLOOD_TYPE bt;          /* organ blood type */
    DONOR_TYPE dt;
    struct organ *next;
} organ;

/** This is a struct that represents the organs that were matched with a patient */
typedef struct matched {
    organ organ;
    patient patient;
} matched;

/** ---------------------------------------------- CENTERS STRUCTS ----------------------------------------------------
 * The system models 4 centers:
 *  - Waiting lists: patients that arrive to the system with active and critical priority are added in the waiting list
 *  - Organ bank: organs that arrive to the system can be dispatched to a patient or added to the organ bank
 *  - Transplant: matching organs and patients are sent to the transplant center
 *  - Activation: patients with transplant incompatible characteristics (inactive priority) must wait a given delay before
 *                being added to the waiting list
 */

// ----------------------------------------- Waiting lists --------------------------------------------------------
/** This struct models a queue of patients with the same blood type and priority */
typedef struct patient_queue_priority {
    PRIORITY priority;
    patient *queue;     // head
    patient *last;      // tail
    double number;
} patient_queue_priority;

/** This struct models a blood type sub-center which holds the lists of patients with that blood type */
typedef struct patient_queue_blood_type {
    BLOOD_TYPE bt;
    patient_queue_priority *priority_queue[NUM_PRIORITIES];             /* head of the patient priority queues */
    double number;                                                      /* l_{p,BT} */
} patient_queue_blood_type;

/**
 * WAITING LISTS
 *      This struct models the center of the waiting lists, meaning lists of patients waiting for a transplant
 *      divided by blood type
 * */
typedef struct patient_waiting_list {
    patient_queue_blood_type *blood_type_queues[NUM_BLOOD_TYPES];   // blood type sub-centers
    double total_number;                                                /* l_{p} */
    double inter_arrival_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];           /* t_{a,p,BT} */
    double num_arrivals[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double num_completions[NUM_BLOOD_TYPES][NUM_PRIORITIES];
} patient_waiting_list;

// ----------------------------------------- Organ bank --------------------------------------------------------
typedef struct organ_queue {
    BLOOD_TYPE bt;
    organ* queue;               /* head of the organ queue */
    organ* last_organ;
    bool organ_available;       /* true = available, false = unavailable - x_{o,BT} - SPECIFICATION */
    double number;              /* l_{o,BT} */
} organ_queue;

/**
 * ORGAN BANK
 *      This struct models the center of the organ bank, meaning lists of organs available for a transplant
 *      divided by blood type.
 * */
typedef struct organ_bank {
    organ_queue *queues[NUM_BLOOD_TYPES];           /* organ queues for blood type */
    double total_number;                            /* l_{o} */
    double inter_arrival_time[NUM_BLOOD_TYPES];     /* t_{a,o,BT} */
    double num_arrivals[NUM_BLOOD_TYPES];
} organ_bank;

// ----------------------------------------- Transplant --------------------------------------------------------

/**
 * TRANSPLANT CENTER
 *      This struct models the transplant center, meaning the list of pairs (patient,organ) subjected to a transplant
 *      with its outcome.
 * */

typedef struct in_transplant {
    matched* matched;           /* pair (organ, patient) */
    double completion_time;      /* time of the transplant completion - t_{c,trans} */
    struct in_transplant *next;
} in_transplant;

typedef struct transplant_center {
    in_transplant *transplanted_patients;   // list of matches
    in_transplant *last_node;           // reference to the last node in the queue to obtain fast append operations
    double total_number;                                /* l_{trans} */
    double min_transplant;
    double completed_transplants[NUM_BLOOD_TYPES][NUM_PRIORITIES];
    double rejected_transplants[NUM_BLOOD_TYPES][NUM_PRIORITIES];
} transplant_center;

// ---------------------------------------------------- EXTRA --------------------------------------------------------
/* loss queues */
typedef struct patient_lost_queue {
    double number_dead[NUM_BLOOD_TYPES][NUM_PRIORITIES];        /* number dead type bt and priority pr - l_{d,p,BT} */
    double number_renege[NUM_BLOOD_TYPES][NUM_PRIORITIES];      /* number reneging type bt and priority pr - l_{r,p,BT} */
    double reneging_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];      /* t_{r,p,BT} */
    double death_time[NUM_BLOOD_TYPES][NUM_PRIORITIES];         /* t_{d,p,BT} */
} patients_lost;

typedef struct organs_expired_queue {
    double number[NUM_BLOOD_TYPES];             /* number expired type bt */
    double reneging_time[NUM_BLOOD_TYPES];      /* t_{r,o,BT} */
} organs_expired;

#endif