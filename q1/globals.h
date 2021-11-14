#ifndef PROCESSSYNCHRONIZATION_GLOBALS_H
#define PROCESSSYNCHRONIZATION_GLOBALS_H
#include <pthread.h>

extern int seconds;
extern pthread_mutex_t seconds_lock; // Use lock for updating time
#endif								 // PROCESSSYNCHRONIZATION_GLOBALS_H
