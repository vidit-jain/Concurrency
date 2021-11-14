#include "functions.h"
// Students

// Returns whether student has submitted their preferences or not
int checkStudentRegistration(Student *s) {
	// If already submitted, then there's no reason to check time
	if (s->submitted)
		return 1;

	// Using lock to be safe, helps in debugging as well to output correctly
	pthread_mutex_lock(&seconds_lock);

	if (s->submission_time <= seconds)
		s->submitted = 1;

	pthread_mutex_unlock(&seconds_lock);
	return s->submitted;
}

// Simulates students via a thread
void *studentThread(void *arg) {
	Student *s = (Student *)arg;
	// Use a form of busy waiting to wait till you are registered to proceed
	while (!checkStudentRegistration(s))
		;
	printf("Student %d has filled in preferences for course registration\n",
		   s->id);
	return NULL;
}