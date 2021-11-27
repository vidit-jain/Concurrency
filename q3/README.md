# Question 3: Multithreaded Client and Server
## Running the program
- Run `make` in the q3 folder
- Execute `./server` and `./client`

## General
- This code was built upon the code provided in the Tutorial
## Client
- The input was first processed in the client, creating an array of requests
- The `begin_process` function was converted into a thread function.
- The command to be sent is created by removing the time specified to send it, and appending it with the request index instead.
- For each request, we create a thread with the begin_process function as the thread function.
- This function will create a socket for each request, and send the command string created by us into the socket, for the server to eventually read.
- Using pthread_join as we don't want the main function to just end after it creates all the threads, we want to ensure that we send all the requests and receive all the responses.

## Server
- An array of size 101 was used to maintain the dictionary in the server.
- Since the range of keys used is low (<=100), I made 101 keys (0-100), and to modify the array value at a key, we just use the respective lock to modify the array value and unlock again. This prevents modification of one key:value pair by multiple threads simultaneously, while permitting threads to modify different keys at the same time.
- multiple functions were made to perform the requests specified, in the instructions, and these functions returned what was to be sent back to the client. 

## Thead pool
- To implement a thread pool, I created an array of threads that kept running.
- Whenever a request came, it would be added into a queue of clients.
- The threads in the thread pool would wait while the queue is empty, and once signalled would run the next client in queue by using handle_connection.
- In handle connection, the string was received, and was parsed using stringstream, and the parse_queries function implement to figure out the type of request.
- After parsing, the appropriate function is called, the message to be sent back to the client is constructed, and after an artificially put 2 second sleep, the message is sent back to the client and the socket is closed between the client and the server.
