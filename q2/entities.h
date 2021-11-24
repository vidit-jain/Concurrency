#ifndef PROCESSSYNCHRONIZATION_ENTITIES_H
#define PROCESSSYNCHRONIZATION_ENTITIES_H
#include <stdlib.h>
#include <string.h>

typedef struct spectator Spectator;
typedef struct goal Goal;
typedef struct group Group;
struct spectator {
	char name[64];
	int reaching_time;
	int patience;
	int max_pain;
	// 0 - home, 1 - away, 2 - neutral
	int side;
	// 1 - goals scored
	// 2 - time up
	int wakeup_reason;
	int exit_time;
	int current_seating;
	int allowed_zones;
	int queues_entered;
	int queues_left;
	pthread_mutex_t lock;
	pthread_cond_t cond_lock;
};
struct goal {
	int side;
	int time;
	double chance;
};
struct group {
	int group_id;
	int spectator_count;
	Spectator **spectators;
};
Spectator *createSpectator(char *name, int reaching_time, int patience,
						   int max_pain, char side);
Goal *createGoal(char side, int time, double chance);
Group *createGroup(int group_id, int spectator_count);
#endif // PROCESSSYNCHRONIZATION_ENTITIES_H
