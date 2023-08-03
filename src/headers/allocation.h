//
// Created by ladypostit on 04/07/23.
//
#ifndef PMCSN_PROJECT_ALLOCATION_H
#define PMCSN_PROJECT_ALLOCATION_H

#define IDENTICAL(o, p) (o == p)
#define COMPATIBLE(o, p) IDENTICAL(o,p) || (o == O) || (p == AB && (o == A || o == B))
/**
 * Returns a constant array of compatible blood types with the one in input ordered by rarest first
 * */

#ifdef IMPROVEMENT
static BLOOD_TYPE* const get_compatibles[] = {
        [O] = (BLOOD_TYPE[]){AB, B, A, O},
        [A] = (BLOOD_TYPE[]){AB, B, A, O},
        [B] = (BLOOD_TYPE[]){AB, B, A, O},
        [AB] = (BLOOD_TYPE[]){AB, B, A, O}
};
#else
static BLOOD_TYPE* const get_compatibles[] = {
        [O] = (BLOOD_TYPE[]){AB, B, A, O},
        [A] = (BLOOD_TYPE[]){AB, A},
        [B] = (BLOOD_TYPE[]){AB, B},
        [AB] = (BLOOD_TYPE[]){AB}
};
#endif

#ifdef IMPROVEMENT
static const int get_num_compatibles[] = {
        [O] = 4,
        [A] = 4,
        [B] = 4,
        [AB] = 4
};
#else
static const int get_num_compatibles[] = {
        [O] = 4,
        [A] = 2,
        [B] = 2,
        [AB] = 1
};
#endif
#endif //PMCSN_PROJECT_ALLOCATION_H
