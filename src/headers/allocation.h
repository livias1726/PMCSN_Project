//
// Created by ladypostit on 04/07/23.
//
#ifndef PMCSN_PROJECT_ALLOCATION_H
#define PMCSN_PROJECT_ALLOCATION_H

#define IDENTICAL(o, p) (o == p)
#define COMPATIBLE(o, p) IDENTICAL(o,p) || (o == O) || (p == AB && (o == A || o == B))

bool ABOCompatible(BLOOD_TYPE organ_type, BLOOD_TYPE patient_type);
bool ABOIdentical(BLOOD_TYPE organ_type, BLOOD_TYPE patient_type);
#endif //PMCSN_PROJECT_ALLOCATION_H
