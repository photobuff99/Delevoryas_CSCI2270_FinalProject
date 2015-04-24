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
#include "Util.h"
}

using std::cin;
using std::cout;
using std::endl;
using std::string;

#define PORTINT 9034
#define PORTSTR "9034"
#define FAMILY AF_UNSPEC
#define TYPE SOCK_STREAM
#define PROTOCOL 0
#define OUTGOING 1
#define PASSIVE 0

int Socket(const char *ip, const char *port, int family, int type, int protocol,
           int direction);
int Connect(const char *ip, const char *port);
void Shell();
ssize_t GetTopic(int fd, struct Topic *topic);
ssize_t Request(int fd, char type, struct Post *p, struct Topic *t);


int main(int argc, char **argv)
{
  Shell();
  cout << "Goodbye!\n";
}

int Socket(const char *ip, const char *port, int family, int type, int protocol,
           int direction)
{
  int sfd, n;
  struct addrinfo hints, *servinfo, *p;
  int yes=1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = family;
  hints.ai_socktype = type;
  if (direction != OUTGOING)
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

    if (direction != OUTGOING) {
      if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
      }
      if (bind(sfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sfd);
        perror("bind");
        continue;
      }
    }

    if (direction == OUTGOING) {
      if (connect(sfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sfd);
        perror("connect");
        continue;
      }
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

int Connect(const char *ip, const char *port)
{
  int sfd;
  sfd = Socket(ip, port, FAMILY, TYPE, PROTOCOL, OUTGOING);
  if (sfd == -1)
    cout << "Error: attempt to connect to host failed\n";
  return sfd;
}

void Shell()
{
  const size_t BUFSIZE = sizeof(struct Request);
  const size_t POSTSIZE = sizeof(struct Post);
  const int INTOPIC = 1;
  const string DEFAULT = "chatclient";
  int servfd = -1;
  int bytes;
  string currenttopic = DEFAULT;
  string line;
  string ip, port;
  int location = 0;
  bool running = true;
  bool connected = false;
  char buffer[POSTSIZE];
  char request_type;
  struct Post post;
  Topic current_topic;
  
  // TODO use hashtable to implement command if/else structure
  while (running) {
    cout << currenttopic << ">";
    getline(cin,line);

    if (line == "connect to host") {
      cout << "chatclient: enter host ip>";
      getline(cin,ip);
      cout << "chatclient: enter port>";
      getline(cin,port);
      servfd = Connect(ip.c_str(), port.c_str());
      if (servfd != -1)
        connected = true;

    } else if (line == "ls") {
      // get topics
      if (connected) {
        bytes = Request(servfd, GETLISTTOPICS, NULL, NULL);
        if (bytes < sizeof(struct Request)) {
          cout << "chatclient: error, request failed\n";
        } else {
          memset(&buffer, 0, sizeof buffer);
          bytes = readn(servfd, buffer, 20);      // TESTING SIZE
          if (bytes < 20)
            cout << "Error! not all bytes read\n";
          else
            cout << buffer << endl;       // TTSTSSD
        }
      } else {
        cout << "chatclient: currently disconnected from host\n";
      }

    } else if (line == "help") {
      // print commands

    } else if (line.substr(0,2) == "cd") {
      memset(&current_topic, 0, sizeof(current_topic));
      bytes = Request(servfd, GETTOPIC, NULL, &current_topic);
      if (bytes < sizeof(current_topic)) {
        cout << "chatclient: error, requesting topics, request\n";
      } else {
        bytes = readn(servfd, &current_topic, sizeof(current_topic));
        if (bytes < sizeof(current_topic)) {
          cout << "chatclient: error, receiving topics\n";
        } else {
          cout << "Title: " << current_topic.title << endl;
          cout << "Username: " << current_topic.username << endl;
          cout << "Post Username: " << current_topic.posts[0].username << endl;
        }
        
      }

    } else if (line == "exit" || line == "quit") {
      running = false;

  /*
    } else if (line == "update" || line == "u") {
      if (connected) {
        bytes = GetTopic(sfd, &current_topic);
      } else {
        cout << "chatclient: currently disconnected from host\n";
      }
  */

    } else if (line == "post") {
      if (connected) {
        cout << "enter post>";
        getline(cin,line);
        // SEND MESSAGE

      } else // if not connected
        cout << "chatclient: currently disconnected from host\n";

    } else {
      cout << "chatclient: " << line << ": command not found\n";
    }
  }
  
  if (connected)
    close(servfd);
}

ssize_t GetTopic(int fd, Topic *topic)
{
  int bytes;
  //bytes = writen(
  return 1;
}

ssize_t Request(int fd, char type, struct Post *p, struct Topic *t)
{
  ssize_t bytes;
  struct Request request;
  memset(&request, 0, sizeof(struct Request));
  
  if (type == GETLISTTOPICS) {
    request.type = GETLISTTOPICS;
  } else if (type == POSTINTOPIC) {
    request.type = POSTINTOPIC;
    memcpy( &(request.post), p, sizeof(struct Post));
  } else if (type == GETTOPIC) {
    request.type = GETTOPIC;
    memcpy( &(request.topic), t, sizeof(struct Topic));
  }

  bytes = writen(fd, &request, sizeof(struct Request));
  printf("%lu/%lu bytes written\n", bytes, sizeof(struct Request));
  return bytes;
}
