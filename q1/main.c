#include "entities.h"
#include "functions.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

int main() {
	srand(time(0));
	pthread_mutex_init(&seconds_lock, NULL);
	// Input
	scanf("%d %d %d", &student_count, &lab_count, &course_count);

	courses_left = course_count;
	students_left = student_count;

	// Course Input
	courses = (Course **)malloc(course_count * sizeof(Course *));
	for (int i = 0; i < course_count; i++) {
		char name[20];
		double interest;
		int course_max_slot;
		int allowed_labs;
		scanf("%s %lf %d %d", name, &interest, &course_max_slot, &allowed_labs);
		int lab_ids[allowed_labs];
		for (int j = 0; j < allowed_labs; j++)
			scanf("%d", &lab_ids[j]);

		courses[i] = createCourse(name, interest, course_max_slot, lab_ids,
								  allowed_labs, i);
	}

	// Student input
	students = (Student **)malloc(student_count * sizeof(Student *));
	for (int i = 0; i < student_count; i++) {
		double caliber;
		int preferences[3];
		int submission_time;
		scanf("%lf", &caliber);
		for (int j = 0; j < 3; j++)
			scanf("%d", &preferences[j]);
		scanf("%d", &submission_time);
		students[i] = createStudent(caliber, submission_time, preferences, i);
	}
	// Lab input
	iiit_labs = (Lab **)malloc(lab_count * sizeof(Lab *));
	for (int i = 0; i < lab_count; i++) {
		char name[20];
		int ta_count;
		int tutorial_limit;
		scanf("%s %d %d", name, &ta_count, &tutorial_limit);
		iiit_labs[i] = createLab(name, ta_count, tutorial_limit, i);
	}
	pthread_t cleanup;
	pthread_create(&cleanup, NULL, cleanupThread, NULL);
	// Student Threads
	pthread_t student_threads[student_count];
	for (int i = 0; i < student_count; i++) {
		pthread_t currThread;
		pthread_create(&currThread, NULL, studentThread, (void *)students[i]);
		student_threads[i] = currThread;
	}
	// Lab threads
	pthread_t lab_threads[lab_count];
	for (int i = 0; i < lab_count; i++) {
		pthread_t currThread;
		pthread_create(&currThread, NULL, labThread, (void *)iiit_labs[i]);
		lab_threads[i] = currThread;
	}
	// Course threads
	pthread_t course_threads[course_count];
	for (int i = 0; i < course_count; i++) {
		pthread_t currThread;
		pthread_create(&currThread, NULL, courseThread, (void *)courses[i]);
		course_threads[i] = currThread;
	}

	time_t start_time = time(NULL);
	while (students_left) {
		time_t curr_time = time(NULL);
		if (curr_time - start_time > seconds) {
			pthread_mutex_lock(&seconds_lock);
			seconds++;
			pthread_mutex_unlock(&seconds_lock);
		}
	}
	// Making sure all threads terminate normally
	for (int i = 0; i < student_count; i++)
		pthread_join(student_threads[i], NULL);
	for (int i = 0; i < course_count; i++)
		pthread_join(course_threads[i], NULL);
	for (int i = 0; i < lab_count; i++)
		pthread_join(lab_threads[i], NULL);
	pthread_join(cleanup, NULL);
}