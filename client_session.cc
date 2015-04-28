// Peter Delevoryas
#include "client_session.h"
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

using std::cout;
using std::cerr;
using std::endl;
using std::string;

client_session::client_session() : fd(-1), valid(false), connected(false), current_topic(HOME){}

client_session::~client_session()
{
  if (valid && fd > 0)
    close(fd);
}

void client_session::connect_to_server(const char *server_addr_, const char *server_port_)
{

  struct addrinfo hints, *servinfo, *p;
  //char localhost[10] = "127.0.0.1";
  int yes = 1;

  server_addr = string(server_addr_);
  server_port = string(server_port_);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // There's an odd bug with using "localhost" for the address that causes
  // the server to receive a connection, but the client to report a failure.
  // Interestingly, simply converting localhost to 127.0.0.1 fixes it.
  if (strcmp(server_addr_,"localhost") == 0) {
    if (getaddrinfo("127.0.0.1", server_port_, &hints, &servinfo) == -1) {
      cerr << "chatclient: getaddrinfo failed" << endl;
      valid = false;
      return;
    }
  } else {
    if (getaddrinfo(server_addr_, server_port_, &hints, &servinfo) == -1) {
      cerr << "chatclient: getaddrinfo failed" << endl;
      valid = false;
      return;
    }
  }

  // Loop through linked list of possible addresses, attempt to connect.
  // On first successful connection, break from the loop.
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ( (fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      cerr << "chatclient: socket invalid, continuing..." << endl;
      continue;
    }
    if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(fd);
      cerr << "chatclient: error on connect: continuing..." << endl;
      continue;
    }
    break;
  }
  if (p == NULL) { // If connect/socket failed in every case, session is invalid
    cerr << "chatclient: error on getting socket, session invalid" << endl;
    valid = false;
    return;
  }
  // Delete linked list of address results
  freeaddrinfo(servinfo);
  // Success! 
  valid = true;
  connected = true;
}

void client_session::init_prompt()
{
  bool running;
  string command;
  string username;

  // Ask the user to enter a usename
  cout << "chatclient: enter your preferred username: " << endl;
  getline(std::cin,username);
  // If unspecified, make it anonymous
  if (username.length() == 0)
    username = DEFAULTUSERNAME;
  // If system admin, ask for a password

  // Let the user know about help
  cout << "chatclient: enter \"help\" to see a list of commands" << endl;

  running = true;
  while (running) {
    cout << "chatclient>";
    getline(std::cin, command);
    if (command != "exit")
      execute(command, username);
    else
      break;
  }
}

void client_session::execute(std::string command, std::string user)
{
  string name, message, line;

  if (command == "help") {
    cout << "Commands:" << endl;
    cout << "  cd: changes current topic/directory" << endl;
    cout << "  ls: lists topics/messages" << endl;
    cout << "  post: you can post topics or messages" << endl;
    cout << "  exit: exits the chat application" << endl;
    cout << "  for more detailed help on any of these commands,\n"
         << "  enter help [commandname]" << endl;
  } else if (command == "help ls") {
    cout << "  Enter ls while in a topic directory to view the last ten messages\n"
         << "  Enter ls while in the home directory to view the list of topics\n";
  } else if (command == "help cd") {
    cout << "  Enter \"cd\" to return to the home directory\n"
         << "  Enter \"cd _____\" to enter a topic directory\n";
  } else if (command == "help post") {
    cout << "  Enter \"post\" while in a topic directory to post a message\n"
         << "  Enter \"post\" while in the home directory to make a new topic\n";
  
  } else if (command == "ls") { // Either return list of topics or messages
    if (current_topic == HOME)  // list of topics, if in HOME "directory"
      get_topics();
    else
      get_messages(current_topic); // if in a topic, get list of messages
  } else if (command.substr(0,2) == "cd") { // if attempting to change current topic
    if (command.length() > 4) // if trying to enter a different topic
      current_topic = command.substr(3);
    else if (command == "cd") // if trying to return to HOME
      current_topic = HOME;
  } else if (command == "post") { // Asking to post either a message or topic
    if (current_topic == HOME) {  // If in the HOME directory, prompt for post name
      cout << "chatclient: enter the name of the topic you want to create: ";
      getline(std::cin,name);
      if (name.size() > TITLELEN) {
        cout << "chatclient: error, topic length to large: "
             << name.length() << '/' << TITLELEN-1 << endl;
        return;
      } else {
        post_topic(name);
      }
    } else { // If not in HOME, ask for user's message
      cout << "chatclient: enter your message followed by a blank line:" << endl;
      message = ""; // perhaps unnecessary to clear message since execute will return
      do {
        cout << '>';
        getline(std::cin,line);
        message += line + '\n';
      } while (line != "");
      post_message(current_topic, user, message);
    }
  } else if (command == "exit") {
    connected = false;
  } else if (command == "") {
    return;
  } else {
    cout << "chatclient: error, command not found" << endl;
  }
}

void client_session::get_topics()
{
  char buffer[MAXTOPICS*TITLELEN];
  Request request;
  memset(&request, 0, sizeof request);
  memset(&buffer, 0, sizeof buffer);
  request.type = GETLISTTOPICS;
  send_request(&request);
  //get_response(&buffer);
  readn(fd, &buffer, sizeof buffer);
  cout << "========Topics========" << endl;
  for (int i = 0; i < MAXTOPICS; ++i) {
    if (buffer[i*TITLELEN] != '\0')
      printf("%d: %s\n",i+1, &(buffer[i*TITLELEN]));
  }
}

void client_session::get_messages(std::string topic)
{
  if (topic.size() > TITLELEN) {
    cerr << "chatclient: error, topic is too long" << endl;
    return;
  }

  Request request;
  int numposts = 0;
  memset(&request, 0, sizeof request);
  request.type = GETTOPIC;
  // sizeof() -1 because of null terminator at the end
  strncpy(request.topic.title,topic.c_str(),sizeof(request.topic.title)-1);
  send_request(&request);
  //cout << "getting messages for " << topic << endl;
  memset(&request, 0, sizeof request);
  get_response(&request);
  cout << "Topic: " << request.topic.title << endl;
  for (int i = 0; i < MAXPOSTS; ++i) {
    if (request.topic.posts[i].text[0] != '\0') {
      ++numposts;
      print_post(&(request.topic.posts[i]));
    }
  }
  if (numposts == 0)
    cout << "No posts in this topic yet" << endl;
}

void client_session::post_message(std::string topic, std::string username, std::string message)
{
  Request request;
  memset(&request, 0, sizeof request);
  request.type = POSTINTOPIC;
  strncpy(request.post.username, username.c_str(), sizeof(request.post.username));
  strncpy(request.post.text, message.c_str(), sizeof(request.post.text));
  strncpy(request.topic.title, topic.c_str(), sizeof(request.topic.title));
  send_request(&request);
}

void client_session::post_topic(std::string title)
{
  Request request;
  memset(&request, 0, sizeof request);
  request.type = POSTTOPIC;
  strncpy(request.topic.title,title.c_str(),sizeof(request.topic.title));
  send_request(&request);
}

void client_session::send_request(struct Request *request)
{
  if (!connected || !valid) {
    cerr << "chatclient: error, can't send request if disconnected" << endl;
    return;
  }
  writen(fd, request, sizeof(struct Request));
}

void client_session::get_response(struct Request *buffer)
{
  if (!connected || !valid) {
    cerr << "chatclient: error, can't receive if disconnected" << endl;
    return;
  }
  readn(fd, buffer, sizeof(struct Request));
}
