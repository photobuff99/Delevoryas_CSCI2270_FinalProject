// PETER DELEVORYAS
#include "server_session.h"
#include "util.h"
#include "util.cc"    // necessary for compilation
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

using std::string;
using std::cerr;
using std::endl;

// Server initialization
server_session::server_session(std::string ip_addr_, std::string port_)
                              : ip_addr(ip_addr_), port(port_)
{
  int yes = 1;
  struct addrinfo hints, *servinfo, *p;

  // Fill in address info about self,
  // i.e. localhost/127.0.0.1
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // servinfo will now point to a
  // linked list of possible address
  // combinations to use when attempting
  // to construct a listening socket
  if (getaddrinfo(ip_addr.c_str(), port.c_str(), &hints, &servinfo) == -1) {
  //if (getaddrinfo("localhost", "3490", &hints, &servinfo) == -1) {
    cerr << "server: getaddrinfo" << endl;
    valid = false; // indicate failure to create socket_stream
    return;
  }

  // Iterate through the linked list of results
  // until a listening socket is successfully allocated
  // and bound to the local machine's ip address and specified
  // port. Also, silence warnings about reuse of sockets,
  // as failing to do so may result in warnings thrown during
  // testing.
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ( (listening_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      cerr << "server: socket" << endl;
      continue;
    }
    if (setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      cerr << "server: setsockopt" << endl;
      valid = false;
      return;
    }
    if (bind(listening_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(listening_fd);
      cerr << "server :bind" << endl;
      continue;
    }
    break; // Success in binding socket!
  }
  // Couldn't allocate and bind a socket
  if (p == NULL) {
    cerr << "server: allocating/binding socket" << endl;
    valid = false;
    return;
  }
  freeaddrinfo(servinfo);
  // listening_fd is now set to a valid socket, ready to call listen() on
  if (listen(listening_fd, LISTENBACKLOG) == -1) {
    cerr << "listening on socket:" << endl;
    valid = false;
    return;
  }
  // Initiate set of file descriptors
  // First clear them, then add listening fd
  FD_ZERO(&all_fds);
  FD_ZERO(&read_fds);
  FD_SET(listening_fd, &all_fds);
  fd_max = listening_fd;
  // Successfull init of server
  valid = true;
}

server_session::~server_session()
{
  if (valid) {
    for (int i = 0; i <= fd_max; ++i)
      if (FD_ISSET(i, &all_fds))
        close(i);
  }
}

void server_session::init_chat()
{
  while (1) {
    read_fds = all_fds; // copy file descriptors

/*
    if (select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1) {
      cerr << "server: error on select" << endl;
      continue;
    }
*/
    accept_connection();

    for (int i = 0; i <= fd_max; ++i) {
      // If a file desc has new input
      if (FD_ISSET(i, &read_fds)) {
        if (i == listening_fd) { // new client trying to connect
          accept_connection();
        } else { // client wrote to socket
          // open up a socket_stream and find out what's happening
          handle_connection(i);
        }
      }
    }
  }
}

void server_session::accept_connection()
{
  std::cout << "DEBUGGING" << endl;

  int new_fd;
  struct sockaddr_storage cliaddr;
  socklen_t addrlen;

  addrlen = sizeof cliaddr;
  memset(&cliaddr, 0, sizeof cliaddr);
  new_fd = accept(listening_fd, (struct sockaddr *)&cliaddr, &addrlen);
  if (new_fd == -1) {
    cerr << "server: error accepting connection" << endl;
    return;
  } else {
    FD_SET(new_fd, &all_fds);
    if (new_fd > fd_max)
      fd_max = new_fd;
  }
}

void server_session::handle_connection(int fd)
{
  int bytes;
  char buffer[sizeof(struct Request)]; // max size of anything sent

  // clear buffer
  memset(buffer, 0, sizeof buffer);
  // read from socket
  bytes = readn(fd, buffer, sizeof buffer);
  // error checking
  if (bytes == 0) { // client disconnected
    printf("Client on %d has disconnected\n", fd);
    close(fd);
    FD_CLR(fd, &all_fds);
  } else if (bytes == -1) { // readn was interrupted
    cerr << "server: error reading from client\n" << endl;
    close(fd);
    FD_CLR(fd, &all_fds);
  } else { // if bytes read > 0
    printf("type of request: %c\n", buffer[0]);
    //printf("trying to access fields: here's posts[9].username: %s\n",
           //((struct Request *)buffer)->topic.title);
    switch (buffer[0]) {
      case 'T':
        return_topic(fd, ((struct Request *)buffer)->topic.title);
        //print_table();
        break;
      case 'P':
        post_message( &(((struct Request *)buffer)->post), ((struct Request *)buffer)->topic.title);
        print_table();
        break;
      case 'N':
        // post topic
        post_topic( ((struct Request *)buffer)->topic.title);
        break;
      case 'L':
        return_topics(fd);
        break;
      default:
        break;
        // didn't understand
    }
  }

}

void server_session::post_topic(std::string title)
{
  Topic topic;
  memset(&topic, 0, sizeof topic);
  strncpy(topic.title, title.c_str(), sizeof(topic.title));
  table.insert(topic);
  std::cout << "Inserting: " << title << endl;
}

void server_session::post_message(struct Post *post, std::string topic)
{
  table.insert(*post, topic);
}

void server_session::return_topic(int fd, std::string title)
{
  Request response;
  memset(&response, 0, sizeof response);
  table.get(title.c_str(),&(response.topic));
  send_response(fd, &response);
}

void server_session::send_response(int fd, struct Request *response)
{
  int bytes;

  bytes = writen(fd, response, sizeof(struct Request));
  if (bytes < sizeof(struct Request))
    cerr << "server: error, " << bytes << '/' << sizeof(struct Request) << " bytes written" << endl;
}

void server_session::return_topics(int fd)
{
  int bytes;

  char buffer[MAXTOPICS*TITLELEN];
  memset(buffer, 0, sizeof buffer);
  table.gettopics(buffer);
  bytes = writen(fd, buffer, sizeof buffer);
  if (bytes < sizeof buffer)
    cerr << "server: error writing bytes, list of topics" << endl;
}
