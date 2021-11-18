CC=g++
LIBS=-pthread
TARGET=a.out
all : client server 

server: server_prog.cpp
	g++ server_prog.cpp -o server -pthread
client: client.cpp
	g++ client.cpp -o client -pthread

