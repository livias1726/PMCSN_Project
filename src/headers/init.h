#ifndef PMCSN_PROJECT_INIT_H
#define PMCSN_PROJECT_INIT_H

void initializePatientQueue(patient_queue_blood_type **pQueue, BLOOD_TYPE bloodType);
void initializeOrganQueue(organ_queue **pQueue, BLOOD_TYPE bloodType);

patient_waiting_list initialize_waiting_list();
organ_bank initialize_organ_bank();
transplant initialize_transplant_center();
activation initialize_activation_center();
patients_lost initialize_patient_lost_queue();
organs_expired initialize_organs_expired_queue();
patient *new_patient(BLOOD_TYPE bt, PRIORITY pr);
organ *new_organ(BLOOD_TYPE bt);

#endif //PMCSN_PROJECT_INIT_H
