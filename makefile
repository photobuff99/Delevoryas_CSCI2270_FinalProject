all: server client clean

client: client_driver.o client_session.o myutil.o
	g++ client_driver.o client_session.o myutil.o -o client

client_driver.o: client_driver.cc
	g++ -c client_driver.cc

client_session.o: client_session.cc
	g++ -c client_session.cc

server: server_session.o server_driver.o Hash.o myutil.o
	g++ server_driver.o server_session.o Hash.o myutil.o -o server

server_session.o:
	g++ -c server_session.cc

server_driver.o:
	g++ -c server_driver.cc

test: Hash.o myutil.o
	g++ test.cc Hash.cc myutil.o -o test

Hash.o:
	g++ -c Hash.cc

myutil.o:
	g++ -c myutil.cc

clean :
	rm client_session.o myutil.o Hash.o server_driver.o server_session.o client_driver.o
