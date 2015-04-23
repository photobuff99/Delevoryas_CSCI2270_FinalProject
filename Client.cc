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
#define BUFSIZE 256
#define OUTGOING 1
#define PASSIVE 0

int Socket(const char *ip, const char *port, int family, int type, int protocol,
           int direction);
void Shell();

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

int SendMessage(int sfd, string message)
{
  int bytes;
  char buffer[BUFSIZE];
  strcpy(buffer, message.c_str());
  bytes = send(sfd, buffer, sizeof buffer, 0);
  return bytes;
}

string Update(int sfd, string currenttopic)
{
  int bytes;
  if (currenttopic == "chatclient")
    currenttopic = "all";
  string request = "GET " + currenttopic;
  char buffer[BUFSIZE];
  memset(buffer, 0, sizeof buffer);
  bytes = SendMessage(sfd, request); // TODO replace BUFSIZE
    cout << "chatclient: " << bytes << '/' << BUFSIZE << " bytes sent\n";
  bytes = recv(sfd, buffer, sizeof buffer, 0);
    cout << "chatclient: " << bytes << '/' << BUFSIZE << " bytes received\n";
  // If length of message is < 255, recv() will be successful
  return string(buffer);
}

void Shell()
{
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
    } else if (line == "help") {
      // print commands
    } else if (line.substr(0,2) == "cd") {
      // get topics appropriately
    } else if (line == "exit" || line == "quit") {
      running = false;
    } else if (line == "update" || line == "u") {
      if (connected)
        cout << Update(servfd,currenttopic);
      else
        cout << "chatclient: currently disconnected from host\n";
    } else if (line == "send message") {
      if (connected) {
        cout << "enter message>";
        getline(cin,line);
        // SEND MESSAGE
        bytes = SendMessage(servfd,line);
        if (bytes < line.length()+1)
          cout << "error, " << bytes << "/" << line.length()+1 << " bytes sent\n";
        else
          cout << "message sent\n";
      } else
        cout << "chatclient: currently disconnected from host\n";
    } else {
      cout << "chatclient: " << line << ": command not found\n";
    }
  }
  
  if (connected)
    close(servfd);
}
