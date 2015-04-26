#include "server_session.h"

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "Command line arguments required! 1. Port Number" << std::endl;
    exit(1);
  }
  server_session ssn("localhost", argv[1]);
  ssn.init_chat();
}
