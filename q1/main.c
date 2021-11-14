#include "entities.h"
#include "functions.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

int main() {
	// Input
	int student_count, lab_count, course_count;
	scanf("%d %d %d", &student_count, &lab_count, &course_count);

	// Course Input
	Course *courses[course_count];
	for (int i = 0; i < course_count; i++) {
		char name[20];
		double interest;
		int course_max_slot;
		int allowed_labs;
		scanf("%s %lf %d %d", name, &interest, &course_max_slot, &allowed_labs);
		int lab_ids[allowed_labs];
		for (int j = 0; j < allowed_labs; j++)
			scanf("%d", &lab_ids[j]);

		courses[i] =
			createCourse(name, interest, course_max_slot, lab_ids, lab_count);
	}

	// Student input
	Student *students[student_count];
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
	// Student Threads
	pthread_t student_threads[student_count];
	for (int i = 0; i < student_count; i++) {
		pthread_create(&student_threads[i], NULL, studentThread,
					   (void *)students[i]);
	}

	time_t start_time = time(NULL);
	while (1) {
		time_t curr_time = time(NULL);
		if (curr_time - start_time > seconds) {
			pthread_mutex_lock(&seconds_lock);
			seconds++;
			printf("t = %d\n", seconds); // Debugging purposes
			pthread_mutex_unlock(&seconds_lock);
		}
	}
}