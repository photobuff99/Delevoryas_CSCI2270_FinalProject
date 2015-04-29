#include "server_session.h"
#include <signal.h>
#include <iostream>

server_session *ssn; // Must be declared globally for signal handler to work

// Handles kill signal, ensuring that the server sessions's table destructor is called
// This is important, as otherwise the binary file will not be written before the program
// is terminated.
void ExitHandler(int x)
{
  delete ssn;
  exit(0);
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "Command line arguments required! 1. Port Number" << std::endl;
    exit(1);
  }

  static struct sigaction _sigact;
  signal(SIGINT, ExitHandler); // SIGINT = CTRL-C
  signal(SIGTERM, ExitHandler); // SIGTERM = kill pid
  std::cout << "Server session being created..." << std::endl;
  ssn = new server_session(argv[1]); // Make a new server session with specified port from command line arg
  if (ssn->is_valid()) { // if valid, initiate the client handling process
    std::cout << "Server session created successfully, accepting clients..." << std::endl;
    ssn->init_chat();
  } else { // else, write error message, delete object, exit program
    std::cout << "Error creating server session, quitting..." << std::endl;
  }
  delete ssn;
}
