all: Client Server

Client: Client.cc Util.o
	g++ Client.cc Util.o -o client -g

Server: Server.cc Util.o
	g++ Server.cc Util.o -o server -g

Util.o: Util.c
	gcc Util.c -c -g
