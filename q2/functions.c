#include "functions.h"
typedef struct wrapper Wrapper;
struct wrapper {
	Spectator *spec;
	int choice;
};
char *suffix(int x) {
	x %= 10;
	if (x == 1)
		return "st";
	if (x == 2)
		return "nd";
	if (x == 3)
		return "rd";
	return "th";
}
int scored(double chance) {
	int chance_int = (int)(chance * 1000);
	int randomNumber = rand() % 1000 + 1;
	return randomNumber <= chance_int;
}
void *goalThread(void *args) {
	int curr_goal_chance = 0;
	while (curr_goal_chance < goal_count) {
		int trigger = -1;
		// Wait till it's time for the next opportunity
		pthread_mutex_lock(&seconds_lock);
		while (seconds_elapsed < goals[curr_goal_chance]->time) {
			pthread_cond_wait(&goal_cond, &seconds_lock);
		}
		pthread_mutex_unlock(&seconds_lock);

		// If the team scores
		if (scored(goals[curr_goal_chance]->chance)) {
			trigger = goals[curr_goal_chance]->side;

			pthread_mutex_lock(&goal_lock);
			goals_scored[trigger]++;
			pthread_mutex_unlock(&goal_lock);

			pthread_mutex_lock(&print_lock);
			printf("t = %d: Team %c scored their %d%s goal\n", seconds_elapsed,
				   zone_symbol[trigger], goals_scored[trigger],
				   suffix(goals_scored[trigger]));
			pthread_mutex_unlock(&print_lock);
		}
		// If the team doesn't score
		else {
			int x = goals[curr_goal_chance]->side;
			pthread_mutex_lock(&print_lock);
			pthread_mutex_lock(&goal_lock);
			printf(
				"t = %d: Team %c missed the chance to score their %d%s goal\n",
				seconds_elapsed, zone_symbol[x], goals_scored[x] + 1,
				suffix(goals_scored[x] + 1));
			pthread_mutex_unlock(&goal_lock);
			pthread_mutex_unlock(&print_lock);
		}
		// Go to next goal opportunity
		curr_goal_chance++;

		// If no goal was scored there's no reason to wake people up
		if (trigger == -1)
			continue;

		// Waking up all the people that should get enraged
		for (int i = 0; i < num_groups; i++) {
			for (int j = 0; j < groups[i]->spectator_count; j++) {
				Spectator *s = groups[i]->spectators[j];
				pthread_mutex_lock(&s->lock);
				int unlocked = 0;
				if (s->current_seating != -1) {
					if (trigger == 1 - s->side &&
						goals_scored[1 - s->side] >= s->max_pain) {
						s->wakeup_reason = 1;
						unlocked = 1;
						pthread_mutex_unlock(&s->lock);
						pthread_cond_signal(&s->cond_lock);
					}
				}
				if (!unlocked)
					pthread_mutex_unlock(&s->lock);
			}
		}
	}
	return NULL;
}
void *spectatorThread(void *arg) {
	Wrapper *t = (Wrapper *)arg;
	Spectator *s = t->spec;
	int pref = t->choice;

	// Reaching the stadium
	sleep(s->reaching_time);

	// Doing this check to prevent duplicate prints, only one print per
	// spectator
	pthread_mutex_lock(&s->lock);
	s->queues_entered++;
	// The first time the spectator entered any queue, therefore print
	if (s->queues_entered == 1) {
		pthread_mutex_lock(&print_lock);
		printf(RED "t = %d: %s entered the stadium\n" WHT, seconds_elapsed,
			   s->name);
		pthread_mutex_unlock(&print_lock);
	}
	pthread_mutex_unlock(&s->lock);

	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	ts.tv_sec += s->patience;

	// Wait for getting a seat acc to thread's pref
	if (sem_timedwait(&available_seats[pref], &ts) == 0) {
		// We got a seat, time to simulate the spectator watching the match
		pthread_mutex_lock(&s->lock);
		// Making sure that he didn't take up a seat in a diff zone already
		if (s->current_seating == -1) {
			s->current_seating = pref;

			pthread_mutex_lock(&print_lock);
			printf(MAG "t = %d: %s has got a seat in zone %c\n" WHT,
				   seconds_elapsed, s->name, zone_symbol[s->current_seating]);
			s->exit_time = seconds_elapsed + spectating_time;
			pthread_mutex_unlock(&print_lock);

			pthread_mutex_lock(&goal_lock);
			// Checking if he should watch the match at all if the opposing team
			// enraged him enough
			if (s->side != 2 && goals_scored[1 - s->side] >= s->max_pain) {
                // Act like he already woke up if he is enraged
				s->wakeup_reason = 1;
				pthread_mutex_unlock(&s->lock);
			} else {
				pthread_mutex_unlock(&s->lock);
				// Wait for goals to make him enrage or to finish his time
				pthread_cond_wait(&s->cond_lock, &goal_lock);
			}
			pthread_mutex_unlock(&goal_lock);
		} else {
            // Give up if you already had a seat
            sem_post(&available_seats[s->current_seating]);
			pthread_mutex_unlock(&s->lock);
		}
	}

	// Leaving the thread
	pthread_mutex_lock(&s->lock);
	s->queues_left++;
	// Only after everyone left the queue
	if (s->queues_left == s->allowed_zones) {
		// Decreasing the number of people that you need to wait for to leave
		// If you got a seat nowhere, current_seating will remain -1
		pthread_mutex_lock(&print_lock);
		// Was never assigned a seat
		if (s->current_seating == -1) {
			printf(MAG "t = %d: %s could not get a seat\n" WHT, seconds_elapsed,
				   s->name);
		}
		// Thread was triggered cause team scored too many goals
		else if (s->wakeup_reason == 1) {
			printf(GRN "t = %d: %s is leaving due to bad performance of his "
					   "team\n" WHT,
				   seconds_elapsed, s->name);
		}
		// Guy got other plans and left
		else if (s->wakeup_reason == 2) {
			printf(GRN "t = %d: %s watched the match for %d seconds and is "
					   "leaving\n" WHT,
				   seconds_elapsed, s->name, spectating_time);
		}
		// Waiting at the gate
		printf(BLU "t = %d: %s is waiting for their friends at the exit\n" WHT,
			   seconds_elapsed, s->name);
		pthread_mutex_unlock(&print_lock);
		if (s->current_seating != -1)
			sem_post(&available_seats[s->current_seating]);
		s->current_seating = -1;
		s->exit_time = -1;
	}
	pthread_mutex_unlock(&s->lock);
	return NULL;
}

void *groupThread(void *arg) {
	Group *g = (Group *)arg;
	pthread_t spectator_threads[g->spectator_count][3];
	for (int i = 0; i < g->spectator_count; i++) {
		// We can make upto 3 threads per spectator
		Spectator *s = g->spectators[i];
		for (int j = 0; j < 3; j++) {
			// If you should make the spectator enter the jth queue
			// s->side == j obvious as you can enter your own zone
			// neutral fan can enter anywhere
			// Only case left is that a home fan can enter a neutral zone
			if (s->side == j || s->side == 2 || (s->side == 0 && j == 2)) {
				Wrapper *w = (Wrapper *)malloc(sizeof(Wrapper));
				w->spec = s;
				w->choice = j;
				pthread_create(&spectator_threads[i][j], NULL, spectatorThread,
							   (void *)w);
			}
		}
	}
    // Wait for all threads to terminate
	for (int i = 0; i < g->spectator_count; i++) {
		Spectator *s = g->spectators[i];
		for (int j = 0; j < 3; j++)
			if (s->side == j || s->side == 2 || (s->side == 0 && j == 2))
				pthread_join(spectator_threads[i][j], NULL);
	}
	pthread_mutex_lock(&print_lock);
	printf(YEL "Group %d is leaving for dinner\n" WHT, g->group_id + 1);
	pthread_mutex_unlock(&print_lock);

	pthread_mutex_lock(&group_lock);
	groups_left--;
	pthread_mutex_unlock(&group_lock);
	return NULL;
}