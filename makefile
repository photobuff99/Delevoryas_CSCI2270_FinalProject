all: client server

client: client.c
	gcc -std=c89 client.c -o client
server: server.c
	gcc -std=c89 server.c -o server
