#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
/////////////////////////////
#include <iostream>
#include <assert.h>
#include <tuple>
#include <queue>
using namespace std;
/////////////////////////////
#include<bits/stdc++.h>
//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

typedef long long LL;

#define pb push_back
#define debug(x) cout << #x << " : " << x << endl
#define part cout << "-----------------------------------" << endl;

#define MAX_CLIENTS 4
#define PORT_ARG 8001
#define MAX_KEY 101
const int initial_msg_len = 256;
int pool_size;
pthread_cond_t client_lock;
pthread_t *thread_pool;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
queue<int> clients;

map<int, string> dictionary;
pthread_mutex_t key_lock[MAX_KEY];
const LL buff_sz = 1048576;

pair<string, int> read_string_from_socket(const int &fd, int bytes)
{
    std::string output;
    output.resize(bytes);

    int bytes_received = read(fd, &output[0], bytes - 1);
    debug(bytes_received);
    if (bytes_received <= 0)
    {
        cerr << "Failed to read data from socket. \n";
    }

    output[bytes_received] = 0;
    output.resize(bytes_received);
    // debug(output);
    return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s)
{
    // debug(s.length());
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0)
    {
        cerr << "Failed to SEND DATA via socket.\n";
    }

    return bytes_sent;
}
string insertDict(int key, string value) {
    pthread_mutex_lock(&key_lock[key]);        
    if (dictionary.find(key) == dictionary.end()) {
        dictionary[key] = value;        
        pthread_mutex_unlock(&key_lock[key]);        
        return "Insertion successful";
    }
    else {
        pthread_mutex_unlock(&key_lock[key]);        
        return "Key already exists";
    }
}
string deleteDict(int key) {
    pthread_mutex_lock(&key_lock[key]);        
    if (dictionary.find(key) != dictionary.end()) {
        dictionary.erase(key);
        pthread_mutex_unlock(&key_lock[key]);        
        return "Deletion successful";
    }
    else {
        pthread_mutex_unlock(&key_lock[key]);        
        return "No such key exists";
    }

}
string updateDict(int key, string value) {
    pthread_mutex_lock(&key_lock[key]);        
    if (dictionary.find(key) != dictionary.end()) {
        dictionary[key] = value;        
        pthread_mutex_unlock(&key_lock[key]);        
        return value; 
    }
    else {
        pthread_mutex_unlock(&key_lock[key]);        
        return "Key already exists";
    }

}
string concatDict(int key1, int key2) {
    int lowKey = min(key1, key2), highKey = max(key1, key2);
    pthread_mutex_lock(&key_lock[lowKey]);
    pthread_mutex_lock(&key_lock[highKey]);
    if (dictionary.find(key1) != dictionary.end() && dictionary.find(key2) != dictionary.end()) {
        string val1 = dictionary[key1];
        string val2 = dictionary[key2];
        dictionary[key1] += val2;
        dictionary[key2] += val1;
        pthread_mutex_lock(&key_lock[highKey]);
        pthread_mutex_lock(&key_lock[lowKey]);
        return val2 + val1;
    }
    else {
        pthread_mutex_lock(&key_lock[highKey]);
        pthread_mutex_lock(&key_lock[lowKey]);
        return "Concat failed as at least one of the keys does not exist";
    }
}
string fetchDict(int key) {
    pthread_mutex_lock(&key_lock[key]);        
    if (dictionary.find(key) != dictionary.end()) {
        string val = dictionary[key];
        pthread_mutex_unlock(&key_lock[key]);        
        return val;
    }
    else {
        pthread_mutex_unlock(&key_lock[key]);        
        return "Key does not exist";
    }

}
///////////////////////////////
string parseQueries(vector<string>& x) {
    if (x[1] == "insert") {
        int key;
        string value;
        key = atoi(x[2].c_str());
        value = x[3];
        return insertDict(key, value);
    }
    else if (x[1] == "delete") {
        int key;
        key = atoi(x[2].c_str());
        return deleteDict(key);
    }
    else if (x[1] == "update") {
        int key;
        string value;
        key = atoi(x[2].c_str());
        value = x[3];
        return updateDict(key, value);
    }
    else if (x[1] == "concat") {
        int key1, key2;
        key1 = atoi(x[2].c_str());
        key2 = atoi(x[3].c_str());
        return concatDict(key1, key2);
    }
    else if (x[1] == "fetch") {
        int key;
        key = atoi(x[2].c_str());
        return fetchDict(key);
    }
    else {
        return "Invalid query";
    }
}
void handle_connection(int client_socket_fd)
{
    //####################################################

    int received_num, sent_num;

    /* read message from client */
    int ret_val = 1;

    while (true)
    {
        string cmd;
        tie(cmd, received_num) = read_string_from_socket(client_socket_fd, buff_sz);
        stringstream temp(cmd);
        vector<string> x;
        string word;
        while (temp >> word) {
            x.push_back(word);
        }
        string output = parseQueries(x);
        ret_val = received_num;
        if (ret_val <= 0)
        {
            // perror("Error read()");
            printf("Server could not read msg sent from client\n");
            goto close_client_socket_ceremony;
        }
        cout << "Client sent : " << cmd << endl;
        // if (cmd == "exit")
        // {
        //     cout << "Exit pressed by client" << endl;
        //     goto close_client_socket_ceremony;
        // }
        // string msg_to_send_back = "Ack: " + cmd;
        pid_t q = gettid();
        string msg_to_send_back = x[0] + ":" + to_string(q) + ":" + output + "\n"; 

        sleep(2); //Artificial wait
        int sent_to_client = send_string_on_socket(client_socket_fd, msg_to_send_back);
        // debug(sent_to_client);
        if (sent_to_client == -1)
        {
            perror("Error while writing to client. Seems socket has been closed");
            goto close_client_socket_ceremony;
        }
             goto close_client_socket_ceremony;
    }

close_client_socket_ceremony:
    close(client_socket_fd);
    printf(BRED "Disconnected from client" ANSI_RESET "\n");
}
void* threadfunction(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (clients.empty()) {
            pthread_cond_wait(&client_lock, &queue_lock);
        }
        int p = clients.front();
        clients.pop();
        pthread_mutex_unlock(&queue_lock);
        handle_connection(p);
    }
    return NULL;
}
// Hello
int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "invalid argument count\n";
        return 0;
    }
    pool_size = atoi(argv[1]);
    thread_pool = (pthread_t *)malloc(pool_size * sizeof(pthread_t));
    for (int i = 0; i < pool_size; i++) {
        pthread_create(&thread_pool[i], NULL, threadfunction, NULL);
    }
    int i, j, k, t, n;

    int wel_socket_fd, client_socket_fd, port_number;
    socklen_t clilen;

    struct sockaddr_in serv_addr_obj, client_addr_obj;
    wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (wel_socket_fd < 0)
    {
        perror("ERROR creating welcoming socket");
        exit(-1);
    }

    bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj));
    port_number = PORT_ARG;
    serv_addr_obj.sin_family = AF_INET;
    serv_addr_obj.sin_addr.s_addr = INADDR_ANY;
    serv_addr_obj.sin_port = htons(port_number); //process specifies port
    if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj, sizeof(serv_addr_obj)) < 0)
    {
        perror("Error on bind on welcome socket: ");
        exit(-1);
    }

    listen(wel_socket_fd, MAX_CLIENTS);
    cout << "Server has started listening on the LISTEN PORT" << endl;
    clilen = sizeof(client_addr_obj);

    while (1)
    {
        printf("Waiting for a new client to request for a connection\n");
        client_socket_fd = accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
        if (client_socket_fd < 0)
        {
            perror("ERROR while accept() system call occurred in SERVER");
            exit(-1);
        }

        printf(BGRN "New client connected from port number %d and IP %s \n" ANSI_RESET, ntohs(client_addr_obj.sin_port), inet_ntoa(client_addr_obj.sin_addr));
        
        pthread_mutex_lock(&queue_lock);
        clients.push(client_socket_fd);
        pthread_cond_signal(&client_lock);
        pthread_mutex_unlock(&queue_lock);
    }

    close(wel_socket_fd);
    return 0;
}
