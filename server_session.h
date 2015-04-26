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
  server_session(std::string ip_addr_, std::string port_);
 ~server_session();
  void init_chat();
  void return_topic(int fd, std::string title);
  void return_topics(int fd);
  void post_topic(std::string title);
  void post_message(struct Post *post, std::string topic);
  void send_response(int fd, struct Request *response);
  inline bool is_valid() { return valid; }
  inline void print_table() { table.print(); }

};

#endif//PETER_DELEVORYAS_SERVER_SESSION_H
