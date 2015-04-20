all: Client Server
debug: gClient gServer
testing: Test

Client: Client.cc Utilities.cc
	g++ Client.cc Utilities.cc -o Client

Server: Server.cc Utilities.cc
	g++ Server.cc Utilities.cc -o Server

gClient: Client.cc Utilities.cc
	g++ Client.cc Utilities.cc -o Client -g

gServer: Server.cc Utilities.cc
	g++ Server.cc Utilities.cc -o Server -g

Test: test.cc Utilities.cc 
	g++ test.cc Utilities.cc -o test
