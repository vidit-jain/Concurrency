#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
/////////////////////////////
#include <assert.h>
#include <iostream>
#include <queue>
#include <tuple>
using namespace std;
/////////////////////////////
#include <bits/stdc++.h>
// Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

#define MAX_CLIENTS 4
#define SERVER_PORT 8001
#define MAX_KEY 101

int pool_size;
pthread_cond_t client_lock;
pthread_t *thread_pool;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
queue<int> clients;

string dict[101];
int exists[101] = {0};
pthread_mutex_t key_lock[MAX_KEY];
const int buff_sz = 1048576;

// Dictionary Queries
string insertDict(int key, string value) {
	string returnString;

	pthread_mutex_lock(&key_lock[key]);
	if (exists[key] == 0) {
		dict[key] = value;
		returnString = "Insertion successful";
		exists[key] = 1;
	} else {
		returnString = "Key already exists";
	}
	pthread_mutex_unlock(&key_lock[key]);

	return returnString;
}
string deleteDict(int key) {
	string returnString;

	pthread_mutex_lock(&key_lock[key]);
	if (exists[key]) {
		dict[key] = "";
		exists[key] = 0;
		returnString = "Deletion successful";
	} else {
		returnString = "No such key exists";
	}
	pthread_mutex_unlock(&key_lock[key]);

	return returnString;
}
string updateDict(int key, string value) {
	string returnString;

	pthread_mutex_lock(&key_lock[key]);
	if (exists[key]) {
		dict[key] = value;
		returnString = value;
	} else {
		returnString = "Key does not exist";
	}
	pthread_mutex_unlock(&key_lock[key]);

	return returnString;
}
string concatDict(int key1, int key2) {
	string returnString;
	int lowKey = min(key1, key2), highKey = max(key1, key2);

	pthread_mutex_lock(&key_lock[lowKey]);
	pthread_mutex_lock(&key_lock[highKey]);

	if (exists[key1] &&
		exists[key2]) {
		string val1 = dict[key1];
		string val2 = dict[key2];
		dict[key1] += val2;
		dict[key2] += val1;
		returnString = val2 + val1;
	} else {
		returnString =
			"Concat failed as at least one of the keys does not exist";
	}
	pthread_mutex_unlock(&key_lock[highKey]);
	pthread_mutex_unlock(&key_lock[lowKey]);

	return returnString;
}
string fetchDict(int key) {
	string returnString;

	pthread_mutex_lock(&key_lock[key]);
	if (exists[key]) {
		string val = dict[key];
		returnString = val;
	} else {
		returnString = "Key does not exist";
	}
	pthread_mutex_unlock(&key_lock[key]);

	return returnString;
}

string parseQueries(vector<string> &tokens) {
	if (tokens[1] == "insert") {
		int key;
		string value;
		key = atoi(tokens[2].c_str());
		value = tokens[3];
		return insertDict(key, value);
	} else if (tokens[1] == "delete") {
		int key;
		key = atoi(tokens[2].c_str());
		return deleteDict(key);
	} else if (tokens[1] == "update") {
		int key;
		string value;
		key = atoi(tokens[2].c_str());
		value = tokens[3];
		return updateDict(key, value);
	} else if (tokens[1] == "concat") {
		int key1, key2;
		key1 = atoi(tokens[2].c_str());
		key2 = atoi(tokens[3].c_str());
		return concatDict(key1, key2);
	} else if (tokens[1] == "fetch") {
		int key;
		key = atoi(tokens[2].c_str());
		return fetchDict(key);
	} else {
		return "Invalid query";
	}
}

pair<string, int> read_string_from_socket(const int &fd, int bytes) {
	std::string output;
	output.resize(bytes);

	int bytes_received = read(fd, &output[0], bytes - 1);
	if (bytes_received <= 0) {
		cerr << "Failed to read data from socket. \n";
	}

	output[bytes_received] = 0;
	output.resize(bytes_received);
	return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s) {
	int bytes_sent = write(fd, s.c_str(), s.length());
	if (bytes_sent < 0) {
		cerr << "Failed to SEND DATA via socket.\n";
	}

	return bytes_sent;
}

void handle_connection(int client_socket_fd) {

	string request;
	int received_num;

	tie(request, received_num) =
		read_string_from_socket(client_socket_fd, buff_sz);

	if (received_num <= 0) {
		printf("Server could not read msg sent from client\n");
		close(client_socket_fd);
		return;
	}

	// Tokenizing data received from client
	stringstream request_stream(request);
	vector<string> tokens;
	string word;
	while (request_stream >> word)
		tokens.push_back(word);

	string output = parseQueries(tokens);

	pthread_t thread_id = pthread_self();
	string client_message =
		tokens[0] + ":" + to_string(thread_id) + ":" + output + "\n";

	sleep(2); // Artificial wait

	if (send_string_on_socket(client_socket_fd, client_message) == -1) {
		perror("Error while writing to client");
	}

	close(client_socket_fd);
}
void *threadfunction(void *arg) {

	// Not busy wait, as we use a condition variable inside
	while (1) {

		pthread_mutex_lock(&queue_lock);

		// Wait till you are signaled and there are clients in the queue
		while (clients.empty())
			pthread_cond_wait(&client_lock, &queue_lock);
		int socket_fd = clients.front();
		clients.pop();

		pthread_mutex_unlock(&queue_lock);

		handle_connection(socket_fd);
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	// Finding thread pool size
	if (argc != 2) {
		cout << "Invalid argument count\n";
		return 0;
	}
	pool_size = atoi(argv[1]);

	// Creating the threadpool
	thread_pool = (pthread_t *)malloc(pool_size * sizeof(pthread_t));
	for (int i = 0; i < pool_size; i++) {
		pthread_create(&thread_pool[i], NULL, threadfunction, NULL);
	}

	struct sockaddr_in serv_addr_obj, client_addr_obj;
	int wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (wel_socket_fd < 0) {
		perror("ERROR creating welcoming socket");
		exit(-1);
	}

	bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj));

	serv_addr_obj.sin_family = AF_INET;
	serv_addr_obj.sin_addr.s_addr = INADDR_ANY;
	serv_addr_obj.sin_port = htons(SERVER_PORT); // process specifies port
	if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj,
			 sizeof(serv_addr_obj)) < 0) {
		perror("Error on bind on welcome socket: ");
		exit(-1);
	}

	listen(wel_socket_fd, MAX_CLIENTS);

	socklen_t clilen = sizeof(client_addr_obj);

	while (1) {

		int client_socket_fd =
			accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
		if (client_socket_fd < 0) {
			perror("ERROR while accept() system call occurred in SERVER");
			exit(-1);
		}

		// Adding the client fd to the client queue, which will eventually be
		// picked up by the worker threads
		pthread_mutex_lock(&queue_lock);

		clients.push(client_socket_fd);
		pthread_cond_signal(&client_lock);

		pthread_mutex_unlock(&queue_lock);
	}

	close(wel_socket_fd);
	return 0;
}
