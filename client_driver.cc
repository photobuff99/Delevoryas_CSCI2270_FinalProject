#include "client_session.h"

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "Command line arguments required! 1. IP Address, 2. Port Number" << std::endl;
    return -1;
  }
  client_session csn;
  std::cout << "Attemping to connect to " << argv[1] << " on port #" << argv[2] << "..." << std::endl;
  csn.connect_to_server(argv[1], argv[2]);
  if (!csn.is_valid()) {
    std::cout << "Failed to connect, quitting..." << std::endl;
    return -1;
  } else {
    std::cout << "...done! You are now connected." << std::endl;
    csn.init_prompt();
  }
}
