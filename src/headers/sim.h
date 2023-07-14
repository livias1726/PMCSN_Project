#ifndef PMCSN_PROJECT_SIM_H
#define PMCSN_PROJECT_SIM_H

#define START 0.0                       /* initial sim_time                   */
#define STOP 365                        /* terminal (close the door) sim_time - 365*6 */

//double getSmallest(double* values, int len); //TODO: can be a utils macro
double getMinTime(event_list *events);
double getMinActivation(in_activation *inactive);
void initializeEventTime(event_list* events);
void sim(event_list* events, sim_time* t, int* organ_arrived, int* patients_arrived);

#endif //PMCSN_PROJECT_SIM_H
