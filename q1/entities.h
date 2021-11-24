#ifndef PROCESSSYNCHRONIZATION_ENTITIES_H
#define PROCESSSYNCHRONIZATION_ENTITIES_H
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
typedef struct course Course;
typedef struct student Student;
typedef struct ta TA;
typedef struct lab Lab;
struct course {
	int id;
	char *name;
	double interest;
	int course_max_slot;
	int lab_count;
	int *lab_ids;
	pthread_mutex_t lock;

	// Use lock for below variables
	int available;
};
struct student {
	int id;
	double caliber;
	int submission_time;
	int preferences[3];
	int current_preference;
	int selected_course; // Indicates the course selected by student. -1
						 // indicates not selected yet/ no course found
	int assigned_course; // Tells whether it was allocated a tutorial for the
						 // current preference.
	int submitted; // Decides whether the student should be considered for a
				   // tutorial slot
	pthread_mutex_t lock;
	pthread_cond_t condition_lock;
};
struct ta {
	int id;
	int course_id;
	int tutorialstaken;
	int available;
	pthread_mutex_t lock;
};
struct lab {
	int id;
	char *name;
	TA **tas;
	int ta_count;
	int eligibleTAs;	// Number of TAs left that are allowed to take tutorials
	int tutorial_limit; // Max number of tutorials a TA can take
	pthread_mutex_t lock;
	pthread_cond_t condition_lock;
};
Student *createStudent(double caliber, int submission_time,
					   const int preferences[3], int id);
Lab *createLab(char *name, int ta_count, int tutorial_limit, int id);
Course *createCourse(char *name, double interest, int course_max_slot,
					 const int *lab_ids, int lab_count, int id);
#endif // PROCESSSYNCHRONIZATION_ENTITIES_H
