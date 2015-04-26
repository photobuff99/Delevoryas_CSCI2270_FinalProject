all: server client

test: test.cc Hash.cc
	g++ test.cc Hash.cc -o test

server: server_session.cc Hash.cc
	g++ server_session.cc Hash.cc -o server

client: client_session.cc
	g++ client_session.cc -o client
