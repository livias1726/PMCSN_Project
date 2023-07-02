
#include <stdbool.h>
#include "enums.h"

#ifndef PMCSN_PROJECT_USERS_H
#define PMCSN_PROJECT_USERS_H

#endif //PMCSN_PROJECT_USERS_H

// --------------------------------------------------- MODEL USERS ---------------------------------------------
typedef struct patient {
    BLOOD_TYPE blood_type;
    PRIORITY priority;
    bool is_active;         /* the patient is active (true or false) */
    //TODO: struct patient *next; (???)
} patient;


typedef struct organ {
    BLOOD_TYPE blood_type;
    PRIORITY priority;
    double starting_age;    /* this is a random number that models the fact that the organ is moved in different states */
} organ;

