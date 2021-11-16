#ifndef PROCESSSYNCHRONIZATION_FUNCTIONS_H
#define PROCESSSYNCHRONIZATION_FUNCTIONS_H
#include "entities.h"
#include "globals.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int checkStudentRegistration(Student *s);
void *studentThread(void *arg);
void *courseThread(void *arg);
void *labThread(void *arg);
void *cleanupThread(void *arg);
#endif // PROCESSSYNCHRONIZATION_FUNCTIONS_H
