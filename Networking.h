#ifndef NETWORKING_H
#define NETWORKING_H

// C++ Header Files
#include <iostream>
#include <string>
#include <vector>

// C Header Files
extern "C"
{
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
#include <string.h>
}

// Used to facilitate communication between Server and Client
class Request
{
 private:
  std::string method;     // GET, POST, or PUT
  std::string resource;   // topic to GET, POST to, or PUT
  std::string host;       // IP or www.
  std::string port;       // Port #
  std::string user_post;  // for POST and PUT methods
 public:
  Request(){}
  Request(std::string m, std::string r, std::string h, std::string p)
    : method(m), resource(r), host(h), port(p) {}

  // Returns request in string form, ready to be sent
  std::string ToString();
};

// Used to respond to requests
class Response
{
 private:
  std::string status;                // Status code, HTTP
  std::string descrip;               // English description of status
  int resource_length;               // Byte length of resource to be returned
  std::string topic;                 // Topic name (opt)
  std::vector<std::string> messages; // Topic messages (opt)
 public:
  Response(){}
  Response(std::string s, std::string d, int r)
    : status(s), descrip(d), resource_length(r){}

  // Returns response in string form
  std::string ToString();
};

      /*       SOCKET FUNCTION WRAPPERS         */

// Wraps getaddrinfo() and socket() into a single Socket() call,
// makes it much easier to read the code and eliminates 10-15 lines.
// Use Socket() to call socket() without having to worry about
// getaddrinfo()'s results, which come in a linked list.
// Takes IP address of a server your trying to connect to
int Socket(const char *ip_addr, const char *port, int domain, int type, int protocol);

// sends a message given a socket file descriptor to write to. Uses send() system function.
// Returns number of characters sent. Uses TCP SOCK_STREAM, so it shouldn't lose any data,
// but if it does you'll know.
int Send(int socket_fd, std::string str);

// Receives a message using recv(). If the size of the message is >= 500 bytes (i.e. 500 char)
// then it will expect the message to be send in 500 character chunks. returns the concatenated
// chunks in a string.
std::string Recv(int socket_fd, int num_bytes);


#endif//NETWORKING_H
