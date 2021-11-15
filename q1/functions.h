#ifndef PROCESSSYNCHRONIZATION_FUNCTIONS_H
#define PROCESSSYNCHRONIZATION_FUNCTIONS_H
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "entities.h"
#include "globals.h"

int checkStudentRegistration(Student *s);
void *studentThread(void *arg);
void *courseThread(void *arg);
void *labThread(void *arg);
#endif // PROCESSSYNCHRONIZATION_FUNCTIONS_H
