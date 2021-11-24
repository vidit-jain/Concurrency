#ifndef PROCESSSYNCHRONIZATION_GLOBALS_H
#define PROCESSSYNCHRONIZATION_GLOBALS_H
#include "entities.h"
#include <pthread.h>
#include <semaphore.h>
extern int spectating_time;
extern int num_groups;
extern int goals_scored[2];

extern Goal **goals;
extern int goal_count;
// extern Spectator** spectators;
// extern int spectator_count;
extern pthread_cond_t goal_cond;

extern int seconds_elapsed;
extern pthread_mutex_t seconds_lock;
extern pthread_mutex_t print_lock;
extern pthread_mutex_t goal_lock;

extern sem_t available_seats[3];
extern int capacities[3];
extern Group **groups;
extern char zone_symbol[3];

extern int groups_left;
extern pthread_mutex_t group_lock;

#endif // PROCESSSYNCHRONIZATION_GLOBALS_H
