#include "entities.h"

Spectator *createSpectator(char *name, int reaching_time, int patience,
						   int max_pain, char side) {
	Spectator *s = (Spectator *)malloc(sizeof(Spectator));
	// Values received
	strcpy(s->name, name), s->reaching_time = reaching_time;
	s->patience = patience, s->max_pain = max_pain;

	// Default values
	s->queues_entered = 0;
	s->queues_left = 0;
	s->wakeup_reason = 0;
	s->current_seating = -1;
	s->exit_time = -1;

	switch (side) {
	case 'H': s->side = 0; break;
	case 'A': s->side = 1; break;
	case 'N': s->side = 2; break;
	}

	// For determining how many zone options does the spectator have
	int zone_count[3] = {2, 1, 3};
	s->allowed_zones = zone_count[s->side];
	return s;
}

Goal *createGoal(char side, int time, double chance) {
	Goal *g = (Goal *)malloc(sizeof(Goal));
	g->side = (side == 'A'), g->time = time, g->chance = chance;
	return g;
}

Group *createGroup(int group_id, int spectator_count) {
	Group *g = (Group *)malloc(sizeof(Group));
	g->group_id = group_id, g->spectator_count = spectator_count;
	g->spectators = (Spectator **)malloc(spectator_count * sizeof(Spectator *));
	return g;
}