
#include "users.h"
#include "enums.h"

#define NUM_PATIENT_QUEUES 12
#define NUM_BLOOD_TYPES 4

// -------------------------------------- CENTERS STRUCTS ----------------------------------------------------

typedef struct patient_queue {
    BLOOD_TYPE bt;
    PRIORITY urg;
    patient* queue;
    double interArrival;
    double numArrivals;
    double numInactive;
    double numDeaths;
    double numRenege;
} patient_queue;

typedef struct patient_waiting_list {
    patient_queue* patient_queue_A_Critical;
    patient_queue* patient_queue_B_Critical;
    patient_queue* patient_queue_AB_Critical;
    patient_queue* patient_queue_0_Critical;
    patient_queue* patient_queue_A_Normal;
    patient_queue* patient_queue_B_Normal;
    patient_queue* patient_queue_AB_Normal;
    patient_queue* patient_queue_0_Normal;
    patient_queue* patient_queue_A_Low;
    patient_queue* patient_queue_B_Low;
    patient_queue* patient_queue_AB_Low;
    patient_queue* patient_queue_0_Low;
} patient_waiting_list;

typedef struct organ_queue {
    BLOOD_TYPE bt;
    organ* queue;
    double interArrival;
    double numArrivals;
    double numExpired;
} organ_queue;

typedef struct organ_bank {
    organ_queue* organ_queue_0;
    organ_queue* organ_queue_A;
    organ_queue* organ_queue_B;
    organ_queue* organ_queue_AB;
} organ_bank;

typedef struct matching_center {
    ORGAN_STATUS servers[NUM_BLOOD_TYPES];
} matching_center;

typedef struct transplant_center {
    double completed_transplant;
    double repeated_transplant;
} transplant;