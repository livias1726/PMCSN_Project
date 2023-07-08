#include "headers/utils.h"

/***
 * ------------------------------------------------ ALLOCATION ALGORITHM HANDLERS -------------------------------------
 */

/***
 * Checks if matching organ-patient is possible based on the ABO-compatible allocation algorithm
 * @param organ_type
 * @param patient_type
 * @return
 */

bool ABOCompatible(BLOOD_TYPE organ_type, BLOOD_TYPE patient_type) {
    if (organ_type == O) {
        return true;
    } else if (organ_type == A) {
        if (patient_type == A || patient_type == AB) {
            return true;
        }
    } else if (organ_type == B) {
        if (patient_type == B || patient_type == AB) {
            return true;
        }
    } else {
        /* AB type */
        if (organ_type == patient_type) {
            return true;
        }
    }

    return false;
}

/***
 * Checks if matching organ-patient is possible based on the ABO-identical allocation algorithm
 * @param organ_type
 * @param patient_type
 * @return
 */
bool ABOIdentical(BLOOD_TYPE organ_type, BLOOD_TYPE patient_type) {
    if (organ_type == patient_type) {
        printf("organ-matching possible according to ABO-identical algo\n");
        return true;
    } else {
        return false;
    }
}
