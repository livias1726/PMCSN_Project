#ifndef PMCSN_PROJECT_TIME_H
#define PMCSN_PROJECT_TIME_H

struct time {
    double current;
    double next;
    double last[5];
};

#endif //PMCSN_PROJECT_TIME_H
