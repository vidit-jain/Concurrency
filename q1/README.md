# Question 1: An alternate course allocation portal
## Running the program
- Run `make` in the `q1` folder
- Execute `./a.out`
## Color Coding used to read the output more clearly
Yellow
Student withdrew from course
Student changed preference priority

Blue
Tutorial started
Course allocated seats

Magenta
Student permanently selected course

Cyan
Student allocated a course
TA allocated a course

Red
Course removed from offerings
No students left to TA in a lab
Student couldn't get any of his preferred courses

Green
Student filled preferences
Tutorial completed

## Student thread
- Student waits till he is registered
- The student waits until he gets alloted a seat, and the tutorial finishes, or the course gets withdrawn from the course offerings.
- If he was alloted a seat, we randomly decide whether the student locks the seat or goes for the next preference.
- If it doesn't accept the seat, it goes on to the next preference, till he runs out of preferences.
- If it takes the seat, it breaks the loop for checking preferences, and continues.
-  After exiting the loop, it decrements the number of students that are left, and if the student wasn't able to get a seat the appropriate message is printed.

## Lab thread
- This thread waited until all the students in the lab can't TA anymore, after which the message is printed and the thread terminates.

## Cleanup thread
This thread goes through all the students to check if they are waiting for a course that has been removed from offerings, and if a student is, then they wake that student up so that it can proceed to it's next preference.

## Course thread
- All courses wait for 1 second for students to register, otherwise the first session would always start with 0 TAs.
- While there are labs which have TAs let for the current course, the while loop continues to go through all the labs to find an eligible TA to take a session for the course.
- If a TA is found, increment the number of times the TA has taken a session, and randomly allocate a number of seats.
- After this, go through the list of students and allocate a seat to all those which have the current course as their current preference.
- After allocating seats, the session starts, using an artificial sleep, and after the session is complete all the students in the session are woken up.
- The 1st case is used, hence sessions are allowed to happen even if there aren't any students in the session (0 students attending the tutorial).
- If there are no more TAs left in any lab, then remove the course from offerings, and decrement the number of courses left.

## Main function
- A loop that runs while there are students left to allocate seats to. Once they drop to 0, the loop terminates.
- All the threads are joined to ensure all the threads terminate before the program terminates.
