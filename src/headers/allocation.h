//
// Created by ladypostit on 04/07/23.
//
#ifndef PMCSN_PROJECT_ALLOCATION_H
#define PMCSN_PROJECT_ALLOCATION_H

#define IDENTICAL(o, p) (o == p)
#define COMPATIBLE(o, p) IDENTICAL(o,p) || (o == O) || (p == AB && (o == A || o == B))

static const BLOOD_TYPE* const get_compatible_patient[] = {
        [O] = (BLOOD_TYPE[]){O, A, B, AB},
        [A] = (BLOOD_TYPE[]){A, AB},
        [B] = (BLOOD_TYPE[]){B, AB},
        [AB] = (BLOOD_TYPE[]){AB}
};

static const int get_num_compatible_patients[] = {
        [O] = 4,
        [A] = 2,
        [B] = 2,
        [AB] = 1
};
#endif //PMCSN_PROJECT_ALLOCATION_H
