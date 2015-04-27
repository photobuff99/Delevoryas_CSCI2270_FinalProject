#include "server_session.h"
#include <signal.h>

server_session *ssn; // Must be declared globally for signal handler to work

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

  signal(SIGINT, ExitHandler);
  ssn = new server_session(argv[1]);
  ssn->init_chat();
  delete ssn;
}
