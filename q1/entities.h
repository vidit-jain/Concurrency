#ifndef PROCESSSYNCHRONIZATION_ENTITIES_H
#define PROCESSSYNCHRONIZATION_ENTITIES_H
#include <stdlib.h>
#include <string.h>
typedef struct course Course;
typedef struct student Student;
typedef struct ta TA;
typedef struct lab Lab;
struct course {
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
	int tutorialstaken;
	int available;
};
struct lab {
	char *name;
	TA **tas;
	int ta_count;
	int eligibleTAs;
	int tutorial_limit;
};
Student *createStudent(double caliber, int submission_time,
					   const int preferences[3], int id);
Lab *createLab(char *name, int ta_count, int tutorialLimit);
Course *createCourse(char *name, double interest, int course_max_slot,
					 const int *lab_ids, int lab_count);
#endif // PROCESSSYNCHRONIZATION_ENTITIES_H
