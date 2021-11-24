#include "globals.h"

Lab **iiit_labs;
Student **students;
Course **courses;
pthread_mutex_t seconds_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t course_lock =
	PTHREAD_MUTEX_INITIALIZER; // Use lock for updating time
pthread_mutex_t student_lock =
	PTHREAD_MUTEX_INITIALIZER; // Use lock for updating time
int student_count, lab_count, course_count;
int seconds = 0;
int courses_left = 0;
int students_left = 0;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
