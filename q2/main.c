#include "entities.h"
#include "functions.h"
#include <stdio.h>
int main() {
	srand(time(NULL));
	// Setting the semaphores to capacities of each zone
	for (int i = 0; i < 3; i++) {
		scanf("%d", &capacities[i]);
		sem_init(&available_seats[i], 0, capacities[i]);
	}

	scanf("%d %d", &spectating_time, &num_groups);
	groups = (Group **)malloc(num_groups * sizeof(Group *));
	// Taking input for groups
	for (int i = 0; i < num_groups; i++) {
		int spectator_count;
		scanf("%d", &spectator_count);
		groups[i] = createGroup(i, spectator_count);
		for (int j = 0; j < spectator_count; j++) {
			char name[64];
			char side;
			int reaching_time, patience, max_pain;
			scanf("%s %c %d %d %d", name, &side, &reaching_time, &patience,
				  &max_pain);
			groups[i]->spectators[j] =
				createSpectator(name, reaching_time, patience, max_pain, side);
		}
	}
	// Inputting the goal opportunities
	scanf("%d", &goal_count);
	goals = (Goal **)malloc(goal_count * sizeof(Goal *));
	for (int i = 0; i < goal_count; i++) {
		char side;
		int time;
		double chance;
		scanf(" %c %d %lf", &side, &time, &chance);
		goals[i] = createGoal(side, time, chance);
	}

	// Simulate the goals
	groups_left = num_groups;
	pthread_t goal_thread;
	pthread_create(&goal_thread, NULL, goalThread, NULL);

	pthread_t group_threads[num_groups];
	for (int i = 0; i < num_groups; i++) {
		pthread_create(&group_threads[i], NULL, groupThread, (void *)groups[i]);
	}

	time_t start_time = time(NULL);
	while (groups_left) {
		time_t curr_time = time(NULL);
		if (curr_time - start_time > seconds_elapsed) {
			// Incrementing seconds counter
			pthread_mutex_lock(&seconds_lock);
			seconds_elapsed++;
			// Wakeup the goal thread to see if it's time for a goal chance
			pthread_cond_signal(&goal_cond);

			// Telling people to leave as they watched enough
			for (int i = 0; i < num_groups; i++) {
				Group *g = groups[i];
				for (int j = 0; j < g->spectator_count; j++) {
					Spectator *s = g->spectators[j];
					pthread_mutex_lock(&s->lock);
					if (s->exit_time != -1 && seconds_elapsed >= s->exit_time) {
						s->wakeup_reason = 2;
						pthread_cond_signal(&s->cond_lock);
					}
					pthread_mutex_unlock(&s->lock);
				}
			}
			pthread_mutex_unlock(&seconds_lock);
		}
	}
	for (int i = 0; i < num_groups; i++) {
		pthread_join(group_threads[i], NULL);
	}
	return 0;
}
