all: Client Server

Client: Client.cc ChatUtil.o
	g++ -std=c++11 Client.cc ChatUtil.o -o client -g

Server: Server.cc ChatUtil.o
	g++ -std=c++11 Server.cc ChatUtil.o -o server -g

ChatUtil.o: ChatUtil.c
	gcc ChatUtil.c -c -g

test: test.cc HashTable.cc
	g++ -std=c++11 test.cc HashTable.cc
