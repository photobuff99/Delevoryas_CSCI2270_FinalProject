#include <iostream>

extern "C"
{
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <time.h>
#include "ChatUtil.h"
}

using std::cin;
using std::cout;
using std::endl;
using std::string;

#define PORTSTR "6000"
#define BACKLOG 10

int Socket(const char *ip, const char *port);
ssize_t Respond(int fd, struct Request *request);

// Preface every message sent with the number of bytes in character form

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Requires port as argv[1]\n");
    exit(1);
  }

  // System type sizes, buffer relies on this
  const size_t BUFSIZE = sizeof(struct Request);
  const size_t POSTSIZE = sizeof(struct Post);
  const size_t NBYTES_SIZE_T = sizeof(size_t);

  int listener, newfd, maxfd, bytes; // file descriptors, 
  char buffer[BUFSIZE];
  char *message;
  fd_set master, readfds;
  struct sockaddr_storage cliaddr;
  socklen_t addrlen;

  // Clear file descriptor sets
  FD_ZERO(&master);
  FD_ZERO(&readfds);

  listener = Socket(NULL, argv[1]);
  if (listener == -1)
    perror("Socket");
  
  // Initiate listening on listener socket
  if (listen(listener, BACKLOG) == -1)
    perror("listen");

  // Add listener socket to list of file descriptors
  FD_SET(listener, &master);
  maxfd = listener;

  // Handles clients through multiplexing/multicasting (terminology?? idgaf!!)
  while (1) {
    // Copy set of all fds to readfds
    readfds = master;

    // select() call alters readfds to only include file
    // descriptors that have new information (connection
    // change or a received message/connection request)
    if (select(maxfd+1, &readfds, NULL, NULL, NULL) == -1) {
      perror("select");
      continue;
    }

    // Find out which file descriptors are in readfds,
    // i.e. figure out which ones have new info
    // If this confuses you, look up what a file
    // descriptor is. (It's an integer)
    for (int i = 0; i <= maxfd; ++i) {
      if (FD_ISSET(i, &readfds)) { // File desc with new info
        if (i == listener) { // if listener fd, info = connection
          addrlen = sizeof cliaddr;
          memset(&cliaddr, 0, sizeof cliaddr);
          newfd = accept(listener, (struct sockaddr *)&cliaddr, &addrlen);
          if (newfd == -1) {
            perror("accept");
          } else {
            FD_SET(newfd, &master);
            if (newfd > maxfd)
              maxfd = newfd;
          }
        } else { // if not listener, call recv() to figure out what's up
          memset(buffer, 0, sizeof buffer);
          //bytes = recv(i, buffer, sizeof buffer, 0); // Old, 256 byte buffer
          bytes = readn(i, buffer, sizeof buffer); // Will get bytes up to BUFSIZE
          if (bytes == 0) { // This happens when a client disconnects
            printf("Client on %d has disconnected\n", i);
            close(i);
            FD_CLR(i, &master);
          } else if (bytes == -1) { // Readn must have been interrupted!
            perror("readn");
            close(i);
            FD_CLR(i, &master);
          } else { // bytes > 0, client sent some bytes, MUST BE IN TEXT FORM
            // If client's message isn't a stream of characters, how can
            // the server know what it's receiving? It would just be a stream
            // of random, unreadable bytes, and last I checked it's impossible
            // to parse bytes without a key. ASCII chars are always one byte,
            // so it's easy to parse them, no matter if the sending or recving
            // systems are different! However, you'll notice that I send
            // back info in byte form: that's because I'm relying on
            // the fact that we will both be the same version of linux
            // running on virtual box, so data should be represented the
            // same way. Also, the client will be expecting data based on the
            // type of request it sends, so it know the key to parse the
            // bytes.
            //printf("Message {%s} received from client on %d\n", buffer, i);
            Respond(i, (struct Request *) &buffer);
          }
        }
      }
    }
  }
  for (int i = 0; i <= maxfd; ++i) {
    if (FD_ISSET(i, &master))
      close(i);
  }
}

int Socket(const char *ip, const char *port)
{
  int sfd, n;
  struct addrinfo hints, *servinfo, *p;
  int yes=1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((n = getaddrinfo(ip, port, &hints, &servinfo)) == -1) {
    perror("getaddrinfo");
    return -1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      return -1;
    }

    if (bind(sfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sfd);
      perror("bind");
      continue;
    }

    break;
  }

  if (p == NULL) {
    perror("getting socket");
    return -1;
  }

  freeaddrinfo(servinfo);

  return sfd;
}

ssize_t Respond(int fd, struct Request *request)
{
  ssize_t bytes;
  char type;

  type = request->type;
  if (type == GETLISTTOPICS) {
    printf("Processing request for list of topics...\n");
    char listtopics[20] = " jjjjj uasdf;lkjad\n";
    bytes = writen(fd, listtopics, sizeof(listtopics));
    printf("done\n");

  } else if (type == POSTINTOPIC) {
    printf("Processing post request\n");
    printf("Username: %s\nText: %s\n",
           request->post.username, request->post.text);
    printf("done\n");

  } else if (type == GETTOPIC) {
    printf("Processing request for topic...\n");
    Topic test;
    memset(&test, 0, sizeof(test));
    char teststr[20] = "1234512345123451234"; // TODO STOP TESTING
    memcpy(&(test.title), &teststr, 20);
    memcpy(&(test.username), &teststr, 20);
    memcpy(&(test.posts[0].username), &teststr, 20);
    bytes = writen(fd, &test, sizeof(test));
    printf("done\n");

  } else {
    printf("Error, could not process type of request\n");
    return -1;
  }

  return bytes;
}
