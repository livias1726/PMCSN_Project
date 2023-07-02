#include <stdbool.h>
#include "enums.h"

#ifndef PMCSN_PROJECT_USERS_H
#define PMCSN_PROJECT_USERS_H

// --------------------------------------------------- MODEL USERS ---------------------------------------------
typedef struct patient {
    // PRIORITY priority;   TODO maybe here?
    bool is_active;         /* the patient is active (true or false) */
    struct patient *next;   /* pointer to the next patient in queue */
} patient;


typedef struct organ {
    double starting_age;    /* this is a random number that models the fact that the organ is moved in different states */
    struct organ *next;
} organ;

#endif //PMCSN_PROJECT_USERS_H