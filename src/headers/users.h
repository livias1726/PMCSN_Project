#ifndef PMCSN_PROJECT_USERS_H
#define PMCSN_PROJECT_USERS_H

// --------------------------------------------------- MODEL USERS ---------------------------------------------
typedef struct patient {
    BLOOD_TYPE bt;          /* patient blood type */
    PRIORITY priority;      /* needed to order patients in list based on the priority */
    bool is_active;         /* the patient is active (true or false) */
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
} matched;

#endif //PMCSN_PROJECT_USERS_H