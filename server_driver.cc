#include "server_session.h"

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Command line arguments required! 1. IP Address, 2. Port Number" << std::endl;
    exit(1);
  }
  server_session ssn(argv[1], argv[2]);
  ssn.init_chat();
}
