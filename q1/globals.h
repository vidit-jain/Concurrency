#ifndef PROCESSSYNCHRONIZATION_GLOBALS_H
#define PROCESSSYNCHRONIZATION_GLOBALS_H
#include "entities.h"
#include <pthread.h>
extern int seconds;
extern pthread_mutex_t seconds_lock; // Use lock for updating time
extern Lab **iiit_labs;
extern Student **students;
extern Course **courses;
extern int student_count, lab_count, course_count;
extern pthread_mutex_t course_lock; // Use lock for updating time
extern int courses_left;
extern pthread_mutex_t student_lock; // Use lock for updating time
extern int students_left;
extern pthread_mutex_t print_lock;
#endif // PROCESSSYNCHRONIZATION_GLOBALS_H
