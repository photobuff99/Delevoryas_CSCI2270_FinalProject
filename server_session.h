#ifndef PETER_DELEVORYAS_SERVER_SESSION_H
#define PETER_DELEVORYAS_SERVER_SESSION_H
#include <iostream>
#include "Hash.h"

#define LISTENBACKLOG 10

// Class for creating a server. Constructor allocates a listening socket,
// init_chat() initiates multicast, accept_connection and handle_connection
// are used to handle socket events. Other functions (return_topics, etc)
// are used to handle client requests.
// Example class use:
// server_session *server_ssn = new server_session("9000");
// if (server_ssn->is_valid())
//   server_ssn->init_chat();
// else
//   cout << "Error initiating server session" << endl;
class server_session
{
 private:
  int listening_fd; // file descriptor to accept new connections
  int fd_max;       // highest file descriptor value in all_fds
  fd_set all_fds;   // fds including clients and listening fd
  fd_set read_fds;  // used to call select()
  bool valid;       // bool representing status of server session
  Hash *table;      // hash table containing topics and posts
  void handle_connection(int fd); // handles client requests
  void accept_connection();       // handles new client connections
  
 public:
  // Attempts to allocate a socket and bind it to the given
  // ip address and port numbers. If it fails, valid is set to false.
  // Otherwise, valid is set to true, and listening_fd will refer
  // to a socket listening on the given port number.
  server_session(const char *port);

  // closes the file descriptors that are currently allocated,
  // deletes hash table. Hash table writes state to binary file.
 ~server_session();

  // Initiates an infinite loop that utilizes the select()
  // system call to determine when events on the socket occur.
  // When an event occurs, uses read() to determine type of event,
  // then the appropriate function is called to handle the event.
  // Ideally, fork() or pthreads would be used to handle client
  // requests, but neither method has been implemented.
  // TODO: level: easy: write fork() call to handle client_connections.
  void init_chat();

  // If the topic, identified by "title", is found in the hash table
  // of topics, the topic is sent to the client defined by the file
  // descriptor fd as an array of bytes. No conversion to a common
  // byte representation or data format is used, the Topic struct is
  // sent directly. Extremely unsafe, also extremely effective as
  // we are all using the same compiler and Virtual Machine OS.
  // If topic is not found, no response is sent. Client will
  // nonetheless attempt to read a topic response.
  // TODO: level: medium: implement error message for topic not found
  void return_topic(int fd, std::string title);

  // Returns a list of topics in a character array to the client on "fd".
  // Character array is always MAXTOPICS * TITLELEN, defined in myutil.h.
  // Each title field is TITLELEN bytes long, and the entire field of
  // size TITLELEN is included in the character array transmitted, even
  // if the length of the topic title is less than TITLELEN-1.
  // If an error occurs writing the character array to the client,
  // an error message is written to std::cerr
  void return_topics(int fd);

  // takes a requested topic title to create and creates
  // a new topic entry in the board, through the hash table
  // Creates a new topic entry in the server's hash table with 
  // title = title. Posts are initialized to null.
  void post_topic(std::string title);

  // Takes a client submitted post and submits it to the client
  // specified topic. See Hash::insert(Post *post, std::string topic)
  // for more information on how the post is inserted to the table.
  // For quick reference, it shifts the other elements in the array
  // of posts, overwriting the oldest one in the process, inserting
  // the newest post at the end of the array. memcpy() is used to
  // perform the shifting and inserting.
  void post_message(struct Post *post, std::string topic);

  // Takes the Request struct made by return_topic, which contains
  // the topic data retrieved by Hash::get, and sends it to the client
  // specified by the file descriptor fd. If an error occurs,
  // an error message is written to std::cerr.
  // TODO: level: medium: implement error message for topic not found
  void send_response(int fd, struct Request *response);

  // Returns valid bool, representing if the server session was created successfully
  inline bool is_valid() { return valid; }

  // Prints the current state of the table, printing any topics found and their
  // corresponding posts, if there are any present.
  inline void print_table() { table->print(); }

  // Retrieves username from buffer (presumed to be USERLEN bytes in size)
  // Writes that client's username and ip address and current time
  // to file "user_records.txt"
  record_user(int fd, char *username);
};

#endif//PETER_DELEVORYAS_SERVER_SESSION_H
