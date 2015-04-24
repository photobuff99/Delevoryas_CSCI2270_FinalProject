all: Client Server

Client: Client.cc Util.o
	g++ Client.cc Util.o -o client

Server: Server.cc Util.o
	g++ Server.cc Util.o -o server

Util.o: Util.c
	gcc Util.c -c
