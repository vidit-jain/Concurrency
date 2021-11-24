#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <netinet/in.h>
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
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <tuple>
#include <vector>
using namespace std;
/////////////////////////////

// Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

#define SERVER_PORT 8001

const int buff_sz = 1048576;
pthread_mutex_t print_lock;
string read_string_from_socket(int fd) {
	std::string output;
	output.resize(buff_sz);

	int bytes_received = read(fd, &output[0], buff_sz - 1);
	if (bytes_received <= 0) {
		cerr << "Failed to read data from socket. Seems server has closed "
				"socket\n";
		exit(-1);
	}

	output[bytes_received] = 0;
	output.resize(bytes_received);

	return output;
}

int send_string_on_socket(int fd, const string &s) {
	int bytes_sent = write(fd, s.c_str(), s.length());
	if (bytes_sent < 0) {
		cerr << "Failed to SEND DATA on socket.\n";
		exit(-1);
	}

	return bytes_sent;
}

int get_socket_fd(struct sockaddr_in *ptr) {
	struct sockaddr_in server_obj = *ptr;
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Error in socket creation for CLIENT");
		exit(-1);
	}
	int port_num = SERVER_PORT;

	memset(&server_obj, 0, sizeof(server_obj)); // Zero out structure
	server_obj.sin_family = AF_INET;
	server_obj.sin_port = htons(port_num); // convert to big-endian order

	if (connect(socket_fd, (struct sockaddr *)&server_obj, sizeof(server_obj)) <
		0) {
		perror("Problem in connecting to the server");
		exit(-1);
	}

	return socket_fd;
}
struct userReq {
	int time;
	char *command;
};
void *begin_process(void *arg) {
	struct userReq s = *((struct userReq *)arg);
	string message(s.command);
	int time = s.time;
	sleep(time);
	struct sockaddr_in server_obj;
	int socket_fd = get_socket_fd(&server_obj);

	send_string_on_socket(socket_fd, message);
	string output_msg;
	output_msg = read_string_from_socket(socket_fd);

    pthread_mutex_lock(&print_lock);
	cout << output_msg;
    pthread_mutex_unlock(&print_lock);
	return NULL;
}
int main(int argc, char *argv[]) {

	int request_count;
	cin >> request_count;
	pthread_t user_requests[request_count];
	vector<pair<int, string>> requests;
	for (int i = 0; i < request_count; i++) {
		string command;
		int t;
		cin >> t;
		getline(cin, command);
		requests.push_back({t, command});
	}

	for (int i = 0; i < request_count; i++) {
		string command = to_string(i) + requests[i].second;

		struct userReq *container =
			(struct userReq *)malloc(sizeof(struct userReq));
		container->command =
			(char *)malloc((command.length() + 1) * sizeof(char));
		strcpy(container->command, command.c_str());
		container->time = requests[i].first;

		pthread_create(&user_requests[i], NULL, begin_process,
					   (void *)container);
	}

	// Make sure all are executed before terminating main program
	for (int i = 0; i < request_count; i++) {
		pthread_join(user_requests[i], NULL);
	}
	return 0;
}
