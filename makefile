all: server client

client: client_driver.o client_session.o
	g++ client_driver.o client_session.o -o client

client_driver.o: client_driver.cc
	g++ -c client_driver.cc

client_session.o: client_session.cc
	g++ -c client_session.cc

server: server_session.o server_driver.o Hash.o
	g++ server_driver.o server_session.o Hash.o -o server

server_session.o:
	g++ -c server_session.cc

server_driver.o:
	g++ -c server_driver.cc

Hash.o:
	g++ -c Hash.cc

