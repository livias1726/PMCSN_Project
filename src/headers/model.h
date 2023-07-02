#include "users.h"

#ifndef PMCSN_PROJECT_MODEL_H
#define PMCSN_PROJECT_MODEL_H


#define NUM_PATIENT_QUEUES 12
#define NUM_ORGAN_QUEUES 4
#define NUM_BLOOD_TYPES 4
#define NUM_PRIORITIES 3

// -------------------------------------- CENTERS STRUCTS ----------------------------------------------------

typedef struct patient_queue {
    BLOOD_TYPE bt;
    //PRIORITY urg;             /*TODO: maybe it's better inside the patient struct (we can order jobs in terms of priority inside the queue) */
    patient* queue;             /* head of the patient queue */
    bool patient_available;     /* true = available/not empty, false = unavailable/empty */
    double number;              /* l_{p,BT} */
    double interArrivalTime;    /* t_{a,p,BT} */
    double deathTime;           /* t_{d,p,BT} */
    double renegingTime;        /* t_{r,p,BT} */
} patient_queue;

typedef struct patient_waiting_list {
    patient_queue *queues[NUM_PATIENT_QUEUES];
    double total_number;                        /* l_{p} */
} patient_waiting_list;

typedef struct organ_queue {
    BLOOD_TYPE bt;
    organ* queue;               /* head of the organ queue */
    bool organ_available;       /* true = available, false = unavailable */
    double number;              /* l_{o,BT} */
    double interArrivalTime;    /* t_{a,o,BT} */
    double renegingTime;        /* t_{r,o,BT} */
} organ_queue;

typedef struct organ_bank {
    organ_queue *queues[NUM_ORGAN_QUEUES];      /* organ queues for blood type */
    double total_number;                              /* l_{o} */
} organ_bank;

bool matching_servers[NUM_BLOOD_TYPES]; // the availability of an organ for a given blood type marks a free and ready server

typedef struct transplant_center {
    double total_number;        /* l_{trans} */
    double serviceTime;         /* t_{c,trans} */
} transplant;

typedef struct activation_center {
    double total_number;        /* l_{del} */
    double serviceTime;         /* t_{c,del} */
} activation;

#endif