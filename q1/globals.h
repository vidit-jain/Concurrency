#ifndef PROCESSSYNCHRONIZATION_GLOBALS_H
#define PROCESSSYNCHRONIZATION_GLOBALS_H
#include <pthread.h>
#include "entities.h"
extern int seconds;
extern pthread_mutex_t seconds_lock; // Use lock for updating time
extern Lab** iiit_labs;
extern Student** students;
extern Course** courses;
extern int student_count, lab_count, course_count;
extern pthread_mutex_t course_lock; // Use lock for updating time
extern int courses_left;
#endif								 // PROCESSSYNCHRONIZATION_GLOBALS_H
