#include <iostream>
#include <fstream>
#include "Networking.h"

using std::cerr; using std::cout; using std::endl;

int main()
{
  int socket_fd;
  struct sockaddr_storage server_addr;
  socklen_t size = sizeof(server_addr);

  if ((socket_fd = Socket("localhost", "9034", AF_INET, SOCK_STREAM, 0)) < 0)
    cerr << "socket" << endl;
  cout << socket_fd << endl;
  std::string s = Recv(socket_fd, 2999);
  if (s.length() < 2999) cerr << "recv" << endl;
  std::cout << s.length() << std::endl;
  std::cout << s << std::endl;
}
