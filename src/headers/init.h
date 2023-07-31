#ifndef PMCSN_PROJECT_INIT_H
#define PMCSN_PROJECT_INIT_H

// -------------------------------------------------- Prototypes -----------------------------------------------------
patient_waiting_list initializeWaitingList();
organ_bank initializeOrganBank();
transplant_center initializeTransplantCenter();
patients_lost initializePatientLostQueue();
organs_expired initializeOrgansExpiredQueue();
patient *newPatient(BLOOD_TYPE bt, PRIORITY pr);
organ *newOrgan(BLOOD_TYPE bt, DONOR_TYPE dt);
in_transplant *newTransplant(matched *matched);
matched * newMatched(patient p, organ o);
event_list *initializeEventList();
sim_time initializeTime();
stats * initializeStatistics();
time_integrated_stats * initializeTimeStatistics();
void initializeEventTime(event_list* events);

#endif //PMCSN_PROJECT_INIT_H
