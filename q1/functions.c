#include "functions.h"

char *suffix(int x) {
	x %= 10;
	if (x == 1)
		return "st";
	if (x == 2)
		return "nd";
	if (x == 3)
		return "rd";
	return "th";
}
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
// Determines whether a course is picked permanently by a student
int accepted(double caliber, double interest) {
	int caliber_int = (int)(caliber * 1000);
	int interest_int = (int)(interest * 1000);
	int prob = caliber_int * interest_int;
	int randomNumber = rand() % 1000000 + 1;
	return randomNumber <= prob;
}
// Simulates students via a thread
void *studentThread(void *arg) {
	Student *s = (Student *)arg;
	// Use a form of busy waiting to wait till you are registered to proceed
	while (!checkStudentRegistration(s))
		;
	pthread_mutex_lock(&print_lock);
	printf(GRN"Student %d has filled in preferences for course registration\n"WHT,
		   s->id);
	pthread_mutex_unlock(&print_lock);
	while (s->current_preference != 3) {
		pthread_mutex_lock(&s->lock);
		pthread_cond_wait(&s->condition_lock, &s->lock);
		// Upon receiving a signal, check whether the current preference course
		// was assigned
		if (s->assigned_course) {
			s->assigned_course = 0;
			int course_id = s->preferences[s->current_preference];
			Course *c = courses[course_id];
			// If the student finalizes this course
			if (accepted(s->caliber, c->interest)) {
				s->selected_course = course_id;
				s->current_preference = 3;
				pthread_mutex_lock(&print_lock);
				printf(MAG"Student %d has selected course %s permanently\n"WHT, s->id,
					   c->name);
				pthread_mutex_unlock(&print_lock);
				pthread_mutex_unlock(&s->lock);
				break;
			} else { // If the student withdraws;
				pthread_mutex_lock(&print_lock);
				printf(YEL"Student %d has withdrawn from course %s\n"WHT, s->id,
					   c->name);
				pthread_mutex_unlock(&print_lock);
			}
			// Will only be executed if we don't select course permanently
			// Since the student was assigned this preference, we must try for
			// the next preference
		}
		int prev_course = s->preferences[s->current_preference];
		s->current_preference++;
		if (s->current_preference == 3) {
			pthread_mutex_unlock(&s->lock);
			break;
		}
		int next_course = s->preferences[s->current_preference];

		pthread_mutex_lock(&print_lock);
		printf(YEL"Student %d has changed current preference from %s "
			   "(priority %d) to %s (priority %d)\n"WHT,
			   s->id, courses[prev_course]->name, s->current_preference,
			   courses[next_course]->name, s->current_preference + 1);
		pthread_mutex_unlock(&print_lock);
		pthread_mutex_unlock(&s->lock);
	}

	pthread_mutex_lock(&student_lock);
	students_left--;
	pthread_mutex_unlock(&student_lock);

	// Student ended up not getting
	if (s->selected_course == -1) {
		pthread_mutex_lock(&print_lock);
		printf(RED"Student %d could not get any of his preferred courses\n"WHT,
			   s->id);
		pthread_mutex_unlock(&print_lock);
	}
	return NULL;
}

void *labThread(void *arg) {
	Lab *lab = (Lab *)arg;

	pthread_mutex_lock(&lab->lock);

	pthread_cond_wait(&lab->condition_lock, &lab->lock);
	pthread_mutex_lock(&print_lock);
	printf(RED"Lab %s no longer has students available for TA ship\n"WHT, lab->name);
	pthread_mutex_unlock(&print_lock);

	pthread_mutex_unlock(&lab->lock);

	return NULL;
}
int randomSeatAllocate(int course_max_slots) {
	return rand() % course_max_slots + 1;
}
// Pushes students to go for their next preference in case they are waiting for
// a course that is not being offered anymore
void *cleanupThread(void *arg) {
	// Keep trying to clean up as long as there are students left for contention
	while (students_left) {
		for (int i = 0; i < student_count; i++) {
			Student *s = students[i];
			pthread_mutex_lock(&s->lock);
			// Avoiding students that are already completed
			if (s->current_preference != 3) {
				int course_id = s->preferences[s->current_preference];
				pthread_mutex_lock(&courses[course_id]->lock);
				// Check if the preference the student is waiting for is
				// available or not, if not, then wake up the student.
				if (!courses[course_id]->available) {
					pthread_cond_signal(&s->condition_lock);
				}
				pthread_mutex_unlock(&courses[course_id]->lock);
			}
			pthread_mutex_unlock(&s->lock);
		}
	}
	return NULL;
}
int studentAllocateSeats(Course *c, int max_seats) {
	// Keeps count of number of seats allocated
	int allocated = 0;
	for (int i = 0; i < student_count; i++) {
		Student *s = students[i];
		pthread_mutex_lock(&s->lock);
		if (!s->assigned_course && s->current_preference != 3) {
			int course_pref = s->preferences[s->current_preference];
			// If the student has submitted and this is the course it currently
			// prefers, then assign is
			if (c->id == course_pref && s->submitted) {
				s->assigned_course = 1;
				pthread_mutex_lock(&print_lock);
				printf(CYN"Student %d has been allocated a seat in course %s\n"WHT,
					   s->id, c->name);
				pthread_mutex_unlock(&print_lock);
				allocated++;
				if (allocated == max_seats) {
					pthread_mutex_unlock(&s->lock);
					break;
				}
			}
		}
		pthread_mutex_unlock(&s->lock);
	}
	return allocated;
}
void *courseThread(void *arg) {
	// Let some students register, else all courses will always start with 0
	// attendance tutorials.
	sleep(1);

	Course *course = (Course *)arg;
	int labs_left = 1;

	while (labs_left) {
		// Flag to indicate whether there are any labs with eligible TAs left
		labs_left = 0;
		// Stores the index of the lab of the TA taken, and the index of the TA
		// in the lab
		int labIndex = -1, taIndex;

		for (int i = 0; i < course->lab_count; i++) {
			int lab_id = course->lab_ids[i];
			int tas = iiit_labs[lab_id]->ta_count;

			for (int j = 0; j < tas; j++) {

				TA *curr_ta = iiit_labs[lab_id]->tas[j];
				pthread_mutex_lock(&curr_ta->lock);

				// If there is a TA that hasn't made use of all slots, say that
				// there are TAs left
				if (curr_ta->tutorialstaken <
					iiit_labs[lab_id]->tutorial_limit) {
					labs_left = 1;
				}

				// If a TA is available to take a tutorial, then take it
				if (curr_ta->available) {
					curr_ta->available = 0;
					labIndex = lab_id;
					taIndex = j;
					curr_ta->tutorialstaken++;

					// Indicate TA has been allocated
					pthread_mutex_lock(&print_lock);
					printf(CYN"TA %d from lab %s has been allocated to course %s "
						   "for his %d%s TA ship\n"WHT,
						   j, iiit_labs[lab_id]->name, course->name,
						   curr_ta->tutorialstaken,
						   suffix(curr_ta->tutorialstaken));
					pthread_mutex_unlock(&print_lock);
					curr_ta->course_id = course->id;

					// Check if the TA has any more attempts to take a tutorial
					// If not, reduce the number of eligible TAs of the lab
					if (curr_ta->tutorialstaken ==
						iiit_labs[lab_id]->tutorial_limit) {

						pthread_mutex_lock(&iiit_labs[lab_id]->lock);
						iiit_labs[lab_id]->eligibleTAs--;
						if (iiit_labs[lab_id]->eligibleTAs == 0) {
							pthread_cond_signal(
								&iiit_labs[lab_id]->condition_lock);
						}
						pthread_mutex_unlock(&iiit_labs[lab_id]->lock);
					}
					pthread_mutex_unlock(&curr_ta->lock);
					break;
				}
				pthread_mutex_unlock(&curr_ta->lock);
			}
			// Means that a TA has been picked
			if (labIndex != -1)
				break;
		}
		// This indicates that there might be TAs left, but they weren't
		// available. It's tested if there were TAs left in the while condition
		if (labIndex == -1)
			continue;
		// Allocate slots for course
		int slots = randomSeatAllocate(course->course_max_slot);
		pthread_mutex_lock(&print_lock);
		printf(BLU"Course %s has been allocated %d seats\n"WHT, course->name, slots);
		pthread_mutex_unlock(&print_lock);

		// Fill seats for course with students
		int allocated = studentAllocateSeats(course, slots);
		pthread_mutex_lock(&print_lock);
		printf(BLU"Tutorial has started for course %s with %d seats filled out of "
			   "%d\n"WHT,
			   course->name, allocated, slots);
		pthread_mutex_unlock(&print_lock);
		// Simulate the tutorial
		sleep(5);

		TA *curr_ta = iiit_labs[labIndex]->tas[taIndex];

		pthread_mutex_lock(&curr_ta->lock);
		// Don't make the TA available again if they have exhausted their limit
		if (curr_ta->tutorialstaken != iiit_labs[labIndex]->tutorial_limit)
			curr_ta->available = 1;
		// Finish the tutorial
		pthread_mutex_lock(&print_lock);
		printf(GRN"TA %d from lab %s has completed the tutorial and left the "
			   "course %s\n"WHT,
			   taIndex, iiit_labs[labIndex]->name, course->name);
		pthread_mutex_unlock(&print_lock);
		curr_ta->course_id = -1;
		pthread_mutex_unlock(&curr_ta->lock);

		for (int i = 0; i < student_count; i++) {
			Student *s = students[i];

			pthread_mutex_lock(&s->lock);
			if (s->current_preference != 3) {
				int course_id = s->preferences[s->current_preference];
				if (course->id == course_id && s->assigned_course) {
					pthread_cond_signal(&s->condition_lock);
				}
			}
			pthread_mutex_unlock(&s->lock);
		}
	}
	// If you've reached here, it means that there are no eligible TAs left
	pthread_mutex_lock(&course->lock);
	course->available = 0;
	pthread_mutex_unlock(&course->lock);

	pthread_mutex_lock(&course_lock);
	courses_left--;
	pthread_mutex_unlock(&course_lock);

	pthread_mutex_lock(&print_lock);
	printf(RED"Course %s doesn't have any TA's eligible and is removed from "
		   "course offerings\n"WHT,
		   course->name);
	pthread_mutex_unlock(&print_lock);
	return NULL;
}