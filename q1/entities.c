#include "entities.h"
TA *createTA(int id) {
	TA *t = (TA *)malloc(sizeof(TA));
	t->available = 1;
	t->id = id;
	t->course_id = -1;
	t->tutorialstaken = 0;
	pthread_mutex_init(&t->lock, NULL);
	return t;
}

Student *createStudent(double caliber, int submission_time,
					   const int preferences[3], int id) {
	Student *s = (Student *)malloc(sizeof(Student));
	s->id = id;
	s->caliber = caliber;
	s->submission_time = submission_time;
	for (int i = 0; i < 3; i++)
		s->preferences[i] = preferences[i];
	s->submitted = 0;
	s->current_preference = 0;
	s->selected_course = -1;

	return s;
}

Lab *createLab(char *name, int ta_count, int tutorial_limit, int id) {
	Lab *t = (Lab *)malloc(sizeof(Lab));
	t->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
	strcpy(t->name, name);
	t->ta_count = ta_count;
	t->tas = (TA **)malloc(t->ta_count * sizeof(TA *));
	for (int i = 0; i < ta_count; i++)
		t->tas[i] = createTA(i);
	t->eligibleTAs = t->ta_count;
	t->tutorial_limit = tutorial_limit;
	t->id = id;

	return t;
}

Course *createCourse(char *name, double interest, int course_max_slot,
					 const int *lab_ids, int lab_count, int id) {
	Course *c = (Course *)malloc(sizeof(Course));
	c->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
	strcpy(c->name, name);
	c->interest = interest;
	c->lab_count = lab_count;
	c->lab_ids = (int *)malloc(c->lab_count * sizeof(int));
	for (int i = 0; i < lab_count; i++)
		c->lab_ids[i] = lab_ids[i];
	c->course_max_slot = course_max_slot;
	c->available = 1;
	c->id = id;
	return c;
}