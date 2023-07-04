#ifndef PMCSN_PROJECT_USERS_H
#define PMCSN_PROJECT_USERS_H

// --------------------------------------------------- MODEL USERS ---------------------------------------------
typedef struct patient {
    PRIORITY priority;      /* needed to order patients in list based on the priority */
    bool is_active;         /* the patient is active (true or false) */
    struct patient *next;   /* pointer to the next patient in queue */
    struct patient *prev;   /* pointer to the previous patient in queue - (doubly linked list) */
} patient;


typedef struct organ {
    double starting_age;    /* this is a random number that models the fact that the organ is moved in different states */
    struct organ *next;
} organ;

#endif //PMCSN_PROJECT_USERS_H