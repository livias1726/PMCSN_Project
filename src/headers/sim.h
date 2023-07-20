#ifndef PMCSN_PROJECT_SIM_H
#define PMCSN_PROJECT_SIM_H

#define START 0.0                       /* initial sim_time                   */
#define STOP (365 * 10)                        /* terminal (close the door) sim_time - 365*6 */

double getMinTime(event_list *events);
double getMinActivation(in_activation *inactive);
double getMinTransplant(in_transplant *transplanted);
void finiteSim(event_list *events, sim_time *t, time_integrated_stats *ti_stats);

#endif //PMCSN_PROJECT_SIM_H
