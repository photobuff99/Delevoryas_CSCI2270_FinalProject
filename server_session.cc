// PETER DELEVORYAS
#include "server_session.h"
#include "myutil.h"
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
server_session::server_session(const char *port)
                              : table(NULL), valid(false), listening_fd(-1)
{
  int yes = 1; // Used in setting the socket options
  struct addrinfo hints, *servinfo, *p; // Used in getaddrinfo and binding the listener

  // Fill in address info about self,
  // i.e. localhost/127.0.0.1
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;      // Find IPv4 AND IPv6 addresses
  hints.ai_socktype = SOCK_STREAM;  // Stream socket, i.e. TCP (not UDP)
  hints.ai_flags = AI_PASSIVE;      // Indicates that local address should be used

  // getaddrinfo will take the port and info we entered in hints to get suitable
  // parameters for the socket() and bind() calls
  if (getaddrinfo(NULL, port, &hints, &servinfo) == -1) {
    cerr << "server: getaddrinfo" << endl;
    valid = false; // indicate failure
    return;
  }

  // Iterate through the linked list of results
  // until a listening socket is successfully allocated
  // and bound to the local machine's ip address and specified
  // port. Also, silence warnings about reuse of sockets,
  // as failing to do so may result in warnings thrown during runtime.
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ( (listening_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      cerr << "server: unable to allocate socket..." << endl;
      continue;
    }
    if (setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      cerr << "server: unable to set socket options" << endl;
      valid = false;
      return;
    }
    if (bind(listening_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(listening_fd);
      // TODO: level: easy: add inet_ntop() call, letting server know which address failed.
      cerr << "server: unable to bind socket to address" << endl;
      continue;
    }
    break; // Success in binding socket!
  }
  // Couldn't allocate and bind a socket
  if (p == NULL) {
    cerr << "server: error allocating/binding socket" << endl;
    valid = false;
    return;
  }
  // Delete linked list
  freeaddrinfo(servinfo);
  // Call listen on the newly allocated listening socket
  if (listen(listening_fd, LISTENBACKLOG) == -1) {
    cerr << "server: error on calling listen()" << endl;
    valid = false;
    return;
  }
  // Initiate set of file descriptors
  // First clear them, then add listening fd
  // When new client connections occur, we'll add
  // their fds to the all_fds set
  FD_ZERO(&all_fds);
  FD_ZERO(&read_fds);
  FD_SET(listening_fd, &all_fds);
  fd_max = listening_fd;
  // Successfull init of server
  valid = true;
  if (valid) // construct hash table
    table = new Hash();
}

server_session::~server_session()
{
  // If server was initiated, there are fds to close
  if (valid) {
    for (int i = 0; i <= fd_max; ++i)
      //If the integer corresponds to one of the fds we have in our set
      if (FD_ISSET(i, &all_fds))
        close(i); // close fd
  }
  if (table) { // If the hash table was allocated, delete it
    delete table;
    table = NULL; // make sure accidental calls to table don't do anything
  }
}

// Starts the standard client accept/handling procedure
// Utilizes select() to wait for fd events
void server_session::init_chat()
{
  // daemon process
  while (1) {
    read_fds = all_fds; // copy file descriptors for use in select()

    // select() takes the read_fds set, if there is a new conection or
    // a client has written some bytes to a socket in our read_fds set,
    // then that file descriptor will remain in read_fds. fds that have
    // no new information/events will be removed from the set.
    // This is why it is necessary to keep two sets: one to hold all the
    // fds, and another to use in the select() call, since select will
    // remove some of the file descriptors
    if (select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1) {
      cerr << "server: error on select" << endl;
      continue;
    }
    // A file descriptor is an integer greater than or equal to 0:
    // to find our client descriptors/listener descriptor, just
    // start with 0 and check if it's in the read_fds set.
    // Continue until you get to the maximum fd value:
    // this is why fd_max is necessary.
    for (int i = 0; i <= fd_max; ++i) {
      // If a file desc is in our read_fds set, it must be a client
      // or the listener descriptor, and it must have new info to handle
      if (FD_ISSET(i, &read_fds)) {
        if (i == listening_fd) { // new client trying to connect
          accept_connection();
        } else { // client wrote to socket, or disconnected
          // open up a socket_stream and find out what's happening
          handle_connection(i);
        }
      }
    }
  }
}

// Pre-conditions: Listener was included in the select() results,
// meaning there is a new client attempting to connect on the listener socket
// Use accept() to accept connection: even though it blocks,
// because we know there is client trying to connect, the server will theoretically
// accept the connection immediately, proceding to handle other connections quickly.
void server_session::accept_connection()
{
  int new_fd;                      // file descriptor for the new client
  struct sockaddr_storage cliaddr; // used to hold client information
  socklen_t addrlen;               // will hold byte size of cliaddr

  addrlen = sizeof cliaddr;
  memset(&cliaddr, 0, sizeof cliaddr); // clear the struct
  // accept() will block, which would be bad if we weren't sure that
  // a client is currently trying to connect. However, since we know
  // a client is trying to connect, the accept() call won't block,
  // and other users waiting for responses wil not be kept waiting
  // (for a signficant amount of time, think milliseconds)
  new_fd = accept(listening_fd, (struct sockaddr *)&cliaddr, &addrlen);
  if (new_fd == -1) { // if accept() failed for some reason
    cerr << "server: error accepting connection" << endl;
    return;
  } else {
    // add the new connection to the set of file descriptors
    FD_SET(new_fd, &all_fds);
    // update fd_max if necessary
    if (new_fd > fd_max)
      fd_max = new_fd;
  }
}

// Main function to respond to client requests/events
// Pre-condition: client has just written a number of bytes to
// their special file descriptor created by a previous accept() call.
void server_session::handle_connection(int fd)
{
  int bytes; // to represent number of bytes read
  // Request struct is larger than any data that will be sent across connection, so
  // it is used to construct a buffer for transmissions.
  char buffer[sizeof(struct Request)];

  // clear buffer
  memset(buffer, 0, sizeof buffer);
  // read from socket: readn reads (sizeof buffer) bytes from socket (fd) into (buffer)
  bytes = readn(fd, buffer, sizeof buffer);

  if (bytes == 0) { // client disconnected
    printf("Client on %d has disconnected\n", fd);
    close(fd); // close file descriptor
    FD_CLR(fd, &all_fds); // remove from fd set
  } else if (bytes == -1) { // readn was interrupted, client may or may not have disconnected
    // To be safe, require client to reconnect by disconnecting the client from the server
    cerr << "server: error reading from client\n" << endl;
    close(fd); // close file descriptor
    FD_CLR(fd, &all_fds); // remove from fd set
  } else { // if bytes read > 0
    // The type of request is stored in the first byte of the Request struct: see myutil.h.
    // Request.type is a single character, so we can treat it as such
    printf("type of request: %c\n", buffer[0]);
    switch (buffer[0]) { // Based on type of request, respond appropriately
      case 'U': // Client is sending username
        record_user(fd, buffer+1);
        break;
      case 'T': // Client is requesting a topic and its posts
        return_topic(fd, ((struct Request *)buffer)->topic.title); // cast the buffer as a Request struct, access field
        break;
      case 'P': // Client is requesting to post a message to the board
        post_message( &(((struct Request *)buffer)->post), // cast the buffer as a Request struct, access fields
                       ((struct Request *)buffer)->topic.title);
        print_table(); // Debugging
        break;
      case 'N': // Client is requesting to post a new topic to the board
        post_topic( ((struct Request *)buffer)->topic.title);
        break;
      case 'L': // Client is requesting a list of the current topic titles
        return_topics(fd);
        break;
      default: // Error on reading Request.type
        cerr << "server: error interpreting type of request: " << buffer[0] << endl;
        break;
    }
  }
}

// Inserts a new topic in the server's hash table
void server_session::post_topic(std::string title)
{
  Topic topic;
  memset(&topic, 0, sizeof topic); // Ensures posts are empty
  strncpy(topic.title, title.c_str(), sizeof(topic.title)); // Copy title
  table->insert(topic); // Hash::insert() handles the topic insertion
  std::cout << "Inserting: " << title << endl; // Used for debugging server-side
}

// Inserts a new message in the server's hash table if the topic is valid.
// Prints result, server-side only.
void server_session::post_message(struct Post *post, std::string topic)
{
  // See Hash::insert(Post *post, std::string topic), the overloaded
  // version of the standard Hash::insert(const Topic &topic)
  if (table->insert(*post, topic) == -1)
    cerr << "server: error, topic: " << topic << " not in message board" << endl;
  else
    std::cout << "server: successfully posted message to" << topic << endl;
}

// Uses Hash::get to fill response.topic with topic information.
// If topic is invalid, sends back an empty struct (all null values);
void server_session::return_topic(int fd, std::string title)
{
  Request response;
  memset(&response, 0, sizeof response);
  table->get(title.c_str(),&(response.topic)); // won't do anything if invalid topic
  send_response(fd, &response); // transmits reponse directly over socket connection
}

// Writes topic to socket connection as an array of bytes (i.e. character array)
// Prints an error if all of the bytes are not transmitted
void server_session::send_response(int fd, struct Request *response)
{
  int bytes;

  bytes = writen(fd, response, sizeof(struct Request));
  if (bytes < sizeof(struct Request))
    cerr << "server: error, " << bytes << '/' << sizeof(struct Request) << " bytes written" << endl;
}

// Writes a buffer of size MAXTOPICS*TITLELEN to the client socket
// containing the titles of the topics currently in the hash table.
// Even if empty, simply sends an empty array with null values.
// Prints error message if fails to write all bytes to socket
void server_session::return_topics(int fd)
{
  int bytes;

  char buffer[MAXTOPICS*TITLELEN];
  memset(buffer, 0, sizeof buffer);
  table->gettopics(buffer);
  bytes = writen(fd, buffer, sizeof buffer);
  if (bytes < sizeof buffer)
    cerr << "server: error writing bytes, list of topics" << endl;
}

void server_session::record_user(int fd, char *username)
{
  std::string username_str(username);
}
