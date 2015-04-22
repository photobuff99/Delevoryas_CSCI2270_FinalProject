all: Client Server

Client: Client.cc Networking.cc
	g++ Client.cc Networking.cc -o Client

Server: Server.cc Networking.cc
	g++ Server.cc Networking.cc -o Server
