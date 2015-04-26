all: server client

client: client_driver.o client_session.o
	g++ client_driver.o client_session.o

client_driver.o: client_driver.cc
	g++ -c client_driver.cc

client:
	g++ -c client_session.cc

server: 
	g++ server_driver.cc server_session.cc Hash.cc

