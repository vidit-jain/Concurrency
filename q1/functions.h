#ifndef PROCESSSYNCHRONIZATION_FUNCTIONS_H
#define PROCESSSYNCHRONIZATION_FUNCTIONS_H
#include "entities.h"
#include "globals.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

int checkStudentRegistration(Student *s);
void *studentThread(void *arg);
void *courseThread(void *arg);
void *labThread(void *arg);
void *cleanupThread(void *arg);
#endif // PROCESSSYNCHRONIZATION_FUNCTIONS_H
