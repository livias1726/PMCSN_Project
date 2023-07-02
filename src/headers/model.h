
#include "users.h"
#include "enums.h"

#define NUM_PATIENT_QUEUES 12
#define NUM_ORGAN_QUEUES 4
#define NUM_BLOOD_TYPES 4
#define NUM_PRIORITIES 3

// -------------------------------------- CENTERS STRUCTS ----------------------------------------------------

typedef struct patient_queue {
    BLOOD_TYPE bt;
    PRIORITY urg;
    /*TODO: should this be a node in a linked list?? if so it needs to have the 'next' field
     * - since the 'patient' struct already holds 'blood_type' and 'priority', should those be duplicated here?
     */
    patient* queue;
    double interArrival;
    double numArrivals;
    double numInactive;
    double numDeaths;
    double numRenege;
} patient_queue;

/*TODO: think it's better like this:
 * - note that the enums can be used as index in the array: patient_waiting_list[8] -> 8 = B(2) * NUM_PRIORITIES + low(2)
 *
 * patient_queue *patient_waiting_list[NUM_PATIENT_QUEUES];
 * */
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
    organ* queue; //TODO: same concerns as the patient queue
    double interArrival;
    double numArrivals;
    double numExpired;
} organ_queue;

/* TODO: same as before
 *
 * organ_queue *organ_bank[NUM_ORGAN_QUEUES];
 * */
typedef struct organ_bank {
    organ_queue* organ_queue_0;
    organ_queue* organ_queue_A;
    organ_queue* organ_queue_B;
    organ_queue* organ_queue_AB;
} organ_bank;

typedef struct matching_center {
    ORGAN_STATUS servers[NUM_BLOOD_TYPES];  // the availability of an organ for a given blood type marks a free and ready server
} matching_center;

typedef struct transplant_center {
    double completed_transplant;
    double repeated_transplant;
} transplant;