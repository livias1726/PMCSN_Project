//
// Created by ladypostit on 13/07/23.
//

#ifndef PMCSN_PROJECT_SIM_TIME_H
#define PMCSN_PROJECT_SIM_TIME_H

typedef struct sim_time {
    double current;
    double next;
    double last[NUM_EVENTS];
} sim_time;

#endif //PMCSN_PROJECT_SIM_TIME_H
