all: Client Server
hash: test1 test2

Client: Client.cc ChatUtil.o
	g++ -std=c++11 Client.cc ChatUtil.o -o client -g

Server: Server.cc ChatUtil.o
	g++ -std=c++11 Server.cc ChatUtil.o -o server -g

ChatUtil.o: ChatUtil.c
	gcc ChatUtil.c -c -g

test: test.cc HashTable.cc
	g++ -std=c++11 test.cc HashTable.cc

test1: test.cc HashTable.cc
	g++ -std=c++11 test.cc HashTable.cc -o test

test2: test2.cc HashTable.cc
	g++ -std=c++11 test2.cc HashTable.cc -o test2
