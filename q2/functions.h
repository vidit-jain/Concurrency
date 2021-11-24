#ifndef PROCESSSYNCHRONIZATION_FUNCTIONS_H
#define PROCESSSYNCHRONIZATION_FUNCTIONS_H
#include "globals.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
void *goalThread(void *args);
void *groupThread(void *arg);

#endif // PROCESSSYNCHRONIZATION_FUNCTIONS_H
