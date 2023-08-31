typedef struct event_list {
    patient_waiting_list patient_arrival;
    activation_center activation_arrival;
    organ_bank organ_arrival;
    double living_donor_completion[NUM_BLOOD_TYPES];
    transplant_center transplant_arrival;
    organs_expired organs_loss;
    patients_lost patients_loss;
} event_list;