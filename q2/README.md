# Question 2: The Clasico Experience
## Running the program
- Run `make` in the `q2` folder
- Execute `./a.out`

## General
- Semaphores are used to indicate the number of seats available in each zone
- Stored information about the spectators in groups, to make implementing the bonus much easier
- Each group consists of information such as the information about the spectators, the number of spectators in the group, and id of the group (stored as 0-index printed as 1-indexed)
- The spectator struct stores all relevant information
## Goal simulation
- To simulate the match being played, the input of each goal is taken, and it's ran in a different thread called goalThread. In this thread, there is a loop that goes through the entire list of
goal chances, and simulates whether they scored or not. To prevent busy waiting, the thread is made to sleep till a second is past in the while loop of the main function.
- After this, the thread checks if the time has come to simulate the current goal chance. If it does score, the thread goes through the list of all spectators watching the match and wakes up those
who would be enraged by this goal opportunity.

## Group & Spectator simulation
- The group thread is responsible for starting the spectator threads
### Handling complex semaphore waiting for spectators
- We know that there are some fans which are allowed to go to multiple zones. It's difficult to implement code to make a thread to take either one of multiple semaphores.
- Hence, I instead created multiple threads per spectator, one for waiting for a seat in each zone, although they use the same spectator struct and condition variable
- If one of the threads corresponding to a specific spectator wakes up, it first checks if the spectator already got a seat.
- If it did, it gives up the extra seat it took.
- Since there are multiple threads corresponding to one spectator, we must ensure coordination between them.
- To make sure the fan only reaches the stadium once, and leaves only once, we use `queues_entered` and `queues_left` variables.
- Each time we start a thread corresponding to a thread, we increment `queues_entered`, and as we are about to exit we increment
`queues_left`. When `queues_entered == 1`, we say the fan reached the stadium, and when `queues_left` becomes the number of
zones he was allowed in, then he posts the exit message.

### General ideas of Group and Spectator simulation
- We use timedwait to simulate the patience of a spectator. If all of the threads fail to get a seat, then it's printed that the fan couldn't get a seat
- Upon getting a seat, we first check if the fan would get enraged by the current score. If yes, then just skip the waiting and make him leave.
- If not, we use a condition variable and make the thread sleep till the spectating time is over (signalled by the main function after incrementing seconds)
or gets signalled by the goal thread when the opposing team scores a goal that exceeds the fan's limit.
- After this we make the fan leave the stadium and wait at the exit for his group.
- The group thread waits for all the spectators to be done by joining all the threads.
- After they are all over, we print the message that the group is exiting for dinner and terminate the thread.
- A counter keeping track of how many groups of fans are left to leave for dinner is decremented
- Once this becomes 0, the main function loop will stop running, and the simulation will terminate as a whole.


