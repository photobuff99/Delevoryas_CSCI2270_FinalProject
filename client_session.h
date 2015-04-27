#ifndef PETER_DELEVORYAS_CLIENT_SESSION_H
#define PETER_DELEVORYAS_CLIENT_SESSION_H
#include <iostream>
#include "util.h"
#define DEFAULT "chatclientdefault"

class client_session
{
 private:
  int fd;
  bool valid;
  bool connected;
  std::string current_topic;
  std::string server_addr;
  std::string server_port;
 public:
  client_session();
 ~client_session();
  void execute(std::string command, std::string user);
  void init_prompt();
  void connect_to_server(const char *server_addr_, const char *server_port_);
  void get_topics();
  void get_messages(std::string topic);
  void send_request(struct Request *request);
  void get_response(struct Request *buffer);
  void post_message(std::string topic, std::string username, std::string message);
  void post_topic(std::string title);
  inline bool is_valid() { return valid; }
};

#endif//PETER_DELEVORYAS_CLIENT_SESSION_H
