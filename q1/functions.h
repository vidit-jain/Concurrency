#ifndef PROCESSSYNCHRONIZATION_FUNCTIONS_H
#define PROCESSSYNCHRONIZATION_FUNCTIONS_H
#include "entities.h"
#include "globals.h"
#include <pthread.h>
#include <stdio.h>

int checkStudentRegistration(Student *s);
void *studentThread(void *arg);
#endif // PROCESSSYNCHRONIZATION_FUNCTIONS_H
