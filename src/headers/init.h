#ifndef PMCSN_PROJECT_INIT_H
#define PMCSN_PROJECT_INIT_H

patient_waiting_list initializeWaitingList();
organ_bank initializeOrganBank();
transplant initializeTransplantCenter();
activation initializeActivationCenter();
patients_lost initializePatientLostQueue();
organs_expired initializeOrgansExpiredQueue();
patient *newPatient(BLOOD_TYPE bt, PRIORITY pr);
organ *newOrgan(BLOOD_TYPE bt);
in_activation * newInactive(patient* patient, double server_offset);
in_transplant * newTransplant(matched* matched, double server_offset);
matched * newMatched(patient p, organ o);
event_list *initializeEventList();
sim_time initializeTime();
stats * initializeStatistics();
time_integrated_stats * initializeTimeStatistics();
void initializeEventTime(event_list* events);

#endif //PMCSN_PROJECT_INIT_H
