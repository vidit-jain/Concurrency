#include "globals.h"
int spectating_time = 0;
int num_groups = 0;
int goals_scored[2] = {0, 0};
int seconds_elapsed = 0;
Goal **goals;
int goal_count = 0;
int capacities[3] = {0, 0, 0};
char zone_symbol[3] = {'H', 'A', 'N'};
int groups_left = 0;
sem_t available_seats[3];
Group **groups;
pthread_cond_t goal_cond;
pthread_mutex_t seconds_lock;
pthread_mutex_t print_lock;
pthread_mutex_t goal_lock;
pthread_mutex_t group_lock;
