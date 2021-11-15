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

void *labThread(void *arg) {
    Lab* lab = (Lab*) arg;
    while (lab->eligibleTAs)
        ;
    printf("Lab %s no longer has students available for TA ship\n", lab->name);
    return NULL;
}
int seatAllocation(int course_max_slots) {
    return rand() % course_max_slots + 1;
}

void *courseThread(void *arg) {
    Course* course = (Course*) arg;
    int labs_left = 1;

    while (labs_left) {
        labs_left = 0;
        int labIndex = -1, taIndex = -1;
        for (int i = 0; i < course->lab_count; i++) {
            int lab_id = course->lab_ids[i];
            int tas = iiit_labs[lab_id]->ta_count;
            for (int j = 0; j < tas; j++) {
                TA* curr_ta = iiit_labs[lab_id]->tas[j];
                pthread_mutex_lock(&curr_ta->lock);
                // If there is a TA that hasn't made use of all slots, say that there are TAs left
                if (curr_ta->tutorialstaken < iiit_labs[lab_id]->tutorial_limit) {
                    labs_left = 1;
                }
                // If a TA is available to take a tutorial
                if (curr_ta->available) {
                    curr_ta->available = 0;
                    labIndex = lab_id;
                    taIndex = j;
                    curr_ta->tutorialstaken++;
                    curr_ta->course_id = course->id;

                    // Check if the TA has any more attempts to take a tutorial
                    if (curr_ta->tutorialstaken == iiit_labs[lab_id]->tutorial_limit) {
                        pthread_mutex_lock(&iiit_labs[lab_id]->lock);
                        iiit_labs[lab_id]->eligibleTAs--;
                        pthread_mutex_unlock(&iiit_labs[lab_id]->lock);
                    }
                    pthread_mutex_unlock(&curr_ta->lock);
                    break;
                }
                pthread_mutex_unlock(&curr_ta->lock);
            }
            if (labIndex != -1) break;
        }
        // This indicates that there might be TAs left, but they weren't available.
        // It's tested if there were TAs left in the while condition
        if (labIndex == -1) continue;
        // Allocate slots for course
        int slots = seatAllocation(course->course_max_slot);
        printf("Course %s has been allocated %d seats\n", course->name, slots);

        // Fill seats for course with students
        int buf = 0;
        printf("Tutorial has started for course %s with %d seats filled out of %d\n", course->name, buf, slots);

        // Simulate the tutorial
        sleep(2);

        TA* curr_ta = iiit_labs[labIndex]->tas[taIndex];
        pthread_mutex_lock(&curr_ta->lock);
        // Don't make the TA available again if they have exhausted their limit
        if (curr_ta->tutorialstaken != iiit_labs[labIndex]->tutorial_limit)
            curr_ta->available = 1;
        // Finish the tutorial
        printf("TA %d from lab %s has completed the tutorial and left the course %s\n", taIndex, iiit_labs[labIndex]->name, course->name);
        curr_ta->course_id = -1;
        pthread_mutex_unlock(&curr_ta->lock);
    }
    // If you've reached here, it means that there are no eligible TAs left
    course->available = 0;

    pthread_mutex_lock(&course_lock);
    courses_left--;
    pthread_mutex_unlock(&course_lock);

    printf("Course %s doesn't have any TA's eligible and is removed from course offerings\n", course->name);
    return NULL;
}