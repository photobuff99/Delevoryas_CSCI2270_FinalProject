all: Client Server

Client: Client.cc Utilities.cc
	g++ Client.cc Utilities.cc -o Client

Server: Server.cc Utilities.cc
	g++ Server.cc Utilities.cc -o Server
