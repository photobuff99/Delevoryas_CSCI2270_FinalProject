all: Client Server

Client: Client20.cc
	g++ Client20.cc -o client

Server: Server20.cc 
	g++ Server20.cc -o server
