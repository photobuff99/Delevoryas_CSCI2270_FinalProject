#ifndef PETER_DELEVORYAS_CLIENT_SESSION_H
#define PETER_DELEVORYAS_CLIENT_SESSION_H
#include <iostream>
#include "myutil.h"
#define HOME "chatclientdefault"

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
  // Does not try to connect to server, only initiates variables to NULL/empty vals
  client_session();

  // Closes the socket descriptor if it was valid
 ~client_session();

  // Execute a command given by the user
  void execute(std::string command, std::string user);

  // Initiates a continuous while loop that prompts the user for commands
  void init_prompt();

  // Attempts to connect to the server specified by server_addr_
  // on the port specified by server_port_
  void connect_to_server(const char *server_addr_, const char *server_port_);

  // Sends a request for the list of topics, prints the list of topics
  // If there aren't any topics, header will print, but with nothing
  // underneath
  void get_topics();

  // Requests the messages from a certain topic. If the topic is valid
  // i.e., in the server's hash table, a Topic struct will be sent back,
  // and the client parses the struct, printing out the posts. If the
  // topic wasn't actually in the hash table, an empty Topic struct
  // will be sent back, and the client will print out the empty values
  // TODO: change program so client doesn't print out empty values
  void get_messages(std::string topic);

  // Takes a request struct and transmits it to the server (if connected)
  // using writen(), found in myutil.h. Used by get_messages, get_topics,
  // etc to send requests to the server.
  void send_request(struct Request *request);

  // Fills the buffer with bytes received from the server.
  // Used in get_messages() and other methods to receive information.
  void get_response(struct Request *buffer);

  // Sends a request to the server asking to insert a post including
  // the specified username, message, and topic. No return value
  // or error checking, however the user can enter the command
  // to call get_messages for the topic and if the post was successful,
  // it will be found in the list of messages.
  // TODO indicate to user if post was successful or not.
  void post_message(std::string topic, std::string username, std::string message);

  // Sends a request to the server to make a new topic in the hash table.
  // Server will create a new topic if it has not already been created:
  // If specified title matches another topic, however, the topic will not
  // be created. Similar to post_message, errors in this process are not
  // indicated by the server, or to the client.
  // TODO indicate to user if post was successful or not.
  void post_topic(std::string title);

  // Returns status of session, i.e. if connection was successful or has
  // not been attempted.
  inline bool is_valid() { return valid; }
};

#endif//PETER_DELEVORYAS_CLIENT_SESSION_H
