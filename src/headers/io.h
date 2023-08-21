#ifndef PMCSN_PROJECT_IO_H
#define PMCSN_PROJECT_IO_H

#include "stats.h"

#define MAX_LEN 1024

#define OPEN_FILE(f, path) \
    if((f = fopen(path, "w")) == NULL) {                                     \
        fprintf(stderr, "Cannot open output file");                          \
        exit(EXIT_FAILURE);                                                  \
    }

void saveResultsCsv(int iter, stats *statistics, bool batch, int batch_num);
void saveResultsLean(stats * statistics);

#endif //PMCSN_PROJECT_IO_H
