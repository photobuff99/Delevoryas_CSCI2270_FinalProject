#ifndef PETER_DELEVORYAS_SERVER_SESSION_H
#define PETER_DELEVORYAS_SERVER_SESSION_H
#include <iostream>
#include "Hash.h"

#define LISTENBACKLOG 10

class server_session
{
 private:
  int listening_fd; // file descriptor to accept new connections
  int fd_max;
  fd_set all_fds;
  fd_set read_fds;
  bool valid;
  std::string ip_addr;
  std::string port;
  Hash table;
  void handle_connection(int fd);
  void accept_connection();
  
 public:

  // Attempts to allocate a socket and bind it to the given
  // ip address and port numbers. If it fails, valid is set to false.
  // Otherwise, valid is set to true, and listening_fd will refer
  // to a listening socket descriptor
  server_session(std::string ip_addr_, std::string port_);

  // closes the file descriptors that are currently allocated
 ~server_session();

  // Initiates an infinite loop that utilizes the select()
  // call to check when to read and write to file
  // descriptors. Prints an error on select() error;
  void init_chat();
  // Returns a topic, not when it's called, but to the client
  // who has requested a topic.
  void return_topic(int fd, std::string title);
  // Leads to an array of bytes being transmitted to the client overa connection
  // takes a file descriptor so that the topics can be sent
  // over a socket.
  void return_topics(int fd);
  // takes a requested topic title to create and creates
  // a new topic entry in the board, through the hash table
  void post_topic(std::string title);
  // Posts a simple message to the message baord
  void post_message(struct Post *post, std::string topic);
  void send_response(int fd, struct Request *response);
  inline bool is_valid() { return valid; }
  inline void print_table() { table.print(); }

};

#endif//PETER_DELEVORYAS_SERVER_SESSION_H
