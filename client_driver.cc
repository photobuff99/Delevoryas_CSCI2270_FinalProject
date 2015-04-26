#include "client_session.h"

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Command line arguments required! 1. IP Address, 2. Port Number" << std::endl;
    exit(1);
  }
  client_session csn;
  csn.connect_to_server(argv[1], argv[2]);
  csn.init_prompt();
}
