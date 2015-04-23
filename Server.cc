#include <iostream>
#include <vector>

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
}

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#define BUFSIZE 256
#define PORT "9034"
#define FAMILY AF_UNSPEC
#define PROTOCOL 0
#define TYPE SOCK_STREAM
#define BACKLOG 10

vector<string> resources;

int Socket(const char *ip, const char *port, int family, int type, int protocol);
int Echo(int sfd, char *buffer);
int HandleClient(int sfd, char *request, int size, vector<string> &resources);

int main(int argc, char **argv)
{
resources.push_back("legos");
resources.push_back("legos,");
resources.push_back("legos, eglkjadf");
resources.push_back("legosadslfkja");
resources.push_back("legosadsfafd");


  int listener, newfd, maxfd, nbytes;
  char buffer[BUFSIZE];
  char *message;
  fd_set master, readfds;
  struct sockaddr_storage cliaddr;
  socklen_t addrlen;

  FD_ZERO(&master);
  FD_ZERO(&readfds);

  // IPv4, stream, default protocol for stream (TCP)
  listener = Socket("localhost", PORT, FAMILY, TYPE, PROTOCOL);
  if (listener == -1)
    perror("Socket");
  
  // Initiate listening on listener socket
  if (listen(listener, BACKLOG) == -1)
    perror("listen");

  // Add listener socket to list of file descriptors
  FD_SET(listener, &master);
  maxfd = listener;

  //vector<string> topics;

  while (1) {
    // Copy set of all fds to readfds
    readfds = master;

    if (select(maxfd+1, &readfds, NULL, NULL, NULL) == -1) {
      perror("select");
      continue;
    }

    for (int i = 0; i <= maxfd; ++i) {
      if (FD_ISSET(i, &readfds)) {
        if (i == listener) {
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
        } else {
          memset(buffer, 0, sizeof buffer);
          nbytes = recv(i, buffer, sizeof buffer, 0);
          if (nbytes == 0) {
            printf("Client on %d has disconnected\n", i);
            close(i);
            FD_CLR(i, &master);
          } else if (nbytes == -1) {
            perror("recv");
            close(i);
            FD_CLR(i, &master);
          } else { // nbytes > 0
            printf("Message {%s} received from client on %d\n", buffer, i);
            HandleClient(i, buffer, sizeof buffer, resources);
            //Echo(i, buffer);
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

int Socket(const char *ip, const char *port, int family, int type, int protocol)
{
  int sfd, n;
  struct addrinfo hints, *servinfo, *p;
  int yes=1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = family;
  hints.ai_socktype = type;
  if ( (strcmp(ip, "127.0.0.1") == 0) ||
       (strcmp(ip, "localhost") == 0) ||
       (ip == NULL))
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

int Echo(int sfd, char *buffer)
{
  int bytes;
  bytes = send(sfd, buffer, BUFSIZE, 0);
  cout << bytes << '/' << BUFSIZE << " echoed\n";
  return bytes;
}

int SendResource(int sfd, string resource, vector<string> &resources)
{
  int status = -1;
  char buffer[BUFSIZE];
  memset(buffer, 0, sizeof buffer);

  if (resource == "all") {
    for (int i = 0; i < resources.size(); ++i) {
      strcat(buffer, resources[i].c_str());
      strcat(buffer, "\n");
    }
    status = Echo(sfd, buffer);
  } else {
    buffer[0] = '4';
    buffer[1] = '0';
    buffer[2] = '4';
    status = Echo(sfd, buffer);
    if (status == -1)
      return -1;
    cout << "404\n";
  }
  return status;
}

int HandleClient(int sfd, char *request, int size, vector<string> &resources)
{
  int status;
  if (request[size-1] != '\0') {
    cout << "HandleClient: error, request is too large\n";
    return -1;
  }
  string request_str(request);
  if (request_str.substr(0,3) == "GET") {
    status = SendResource(sfd, request_str.substr(4), resources);
    if (status == -1) {
      cout << "HandleClient: SendResource: error\n";
    }
    return status;
  } else {
    cout << "error, request not understood or command not found\n";
    return -1;
  }
}
