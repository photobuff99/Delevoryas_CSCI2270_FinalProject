#include "client_session.h"

int main(int argc, char **argv)
{
  if (argc < 3) { // IP = server, PORT = server's listening socket's port number
    std::cerr << "Command line arguments required! 1. IP Address, 2. Port Number" << std::endl;
    return -1;
  }
  client_session csn; // Create a new client session
  std::cout << "Attemping to connect to " << argv[1] << " on port #" << argv[2] << "..." << std::endl;
  csn.connect_to_server(argv[1], argv[2]); // attempt to connect to the server
  if (!csn.is_valid()) { // If failure to connect to server...
    std::cout << "Failed to connect, quitting..." << std::endl;
    return -1;
  } else { // Else, initiate shell prompt
    std::cout << "...done! You are now connected." << std::endl;
    csn.init_prompt();
  }
}
