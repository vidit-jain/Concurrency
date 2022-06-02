# Concurrency
A project involving solving various types of concurrency problems in C, C++ making use of pthreads mainly.


## Question 1

Implementation of an alternate course system with entities such as Labs, their Teaching Assistants, Students, Courses, and Tutorials, where all entities 
interact with each other asynchronously, yet in a thread-safe manner with no deadlocks, and no usage of any "busy waits" in order to minimize idle CPU time.

## Question 2

Simulation of a real-time seat allocation system in a stadium, which involved solving complex-concurrency problem such a thread
being dependant on multiple semaphores.

## Question 3
Creation of a multithreaded server which can be accessed by clients concurrently, connected using networking. Server makes use of a thread-pool, 
and ensures changes made by clients to the information are done in a threadsafe way.
