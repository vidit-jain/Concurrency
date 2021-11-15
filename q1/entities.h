#ifndef PROCESSSYNCHRONIZATION_ENTITIES_H
#define PROCESSSYNCHRONIZATION_ENTITIES_H
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
	int available;
};
struct student {
	int id;
	double caliber;
	int submission_time;
	int preferences[3];
	int current_preference;
	int selected_course;
	int submitted;
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
	int eligibleTAs;
	int tutorial_limit;
    pthread_mutex_t lock;
};
Student *createStudent(double caliber, int submission_time,
					   const int preferences[3], int id);
Lab *createLab(char *name, int ta_count, int tutorial_limit, int id);
Course *createCourse(char *name, double interest, int course_max_slot,
					 const int *lab_ids, int lab_count, int id);
#endif // PROCESSSYNCHRONIZATION_ENTITIES_H
