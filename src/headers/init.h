#ifndef PMCSN_PROJECT_INIT_H
#define PMCSN_PROJECT_INIT_H

patient_waiting_list initialize_waiting_list();
organ_bank initialize_organ_bank();
transplant initialize_transplant_center();
activation initialize_activation_center();
patients_lost initialize_patient_lost_queue();
organs_expired initialize_organs_expired_queue();
patient *new_patient(BLOOD_TYPE bt, PRIORITY pr);
organ *new_organ(BLOOD_TYPE bt);

event_list initialize_event_list();
sim_time initialize_time();
void initializeStatistics(stats*);
void initializeEventTime(event_list* events);

#endif //PMCSN_PROJECT_INIT_H
