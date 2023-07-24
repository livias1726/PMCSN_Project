#ifndef PMCSN_PROJECT_SIM_H
#define PMCSN_PROJECT_SIM_H

#define BATCH_SIZE 30                       // in terms of number of days
#define BASE_OBSERVATION 0                  // must be at least 3 years to unblock inactive patients
#define INIT (365 * BASE_OBSERVATION)
#define OBSERVATION 3                       // years of system observation after the initial phase
#define START 0.0                           /* initial sim_time                   */
#define STOP (365 * (BASE_OBSERVATION + OBSERVATION))

double getMinTime(event_list *events);
double getMinActivation(in_activation *inactive);
double getMinTransplant(in_transplant *transplanted);
void finiteSim(event_list *events, sim_time *t, time_integrated_stats *ti_stats, stats **batches, stats *final_stat,
               int *num_iterations);

#endif //PMCSN_PROJECT_SIM_H
