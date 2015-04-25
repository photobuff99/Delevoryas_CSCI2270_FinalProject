all: Client Server
hash: test1 test2

Client: Client.cc Hash.cc ChatUtil.o
	g++ -std=c++11 Client.cc Hash.cc ChatUtil.o -o client -g

Server: Server.cc Hash.cc ChatUtil.o
	g++ -std=c++11 Server.cc Hash.cc ChatUtil.o -o server -g

ChatUtil.o: ChatUtil.c
	gcc ChatUtil.c -c -g

test: test.cc Hash.cc
	g++ -std=c++11 test.cc Hash.cc -g -o test

test1: test.cc Hash.cc
	g++ -std=c++11 test.cc Hash.cc -o test -g

test2: test2.cc Hash.cc
	g++ -std=c++11 test2.cc Hash.cc -o test2 -g
