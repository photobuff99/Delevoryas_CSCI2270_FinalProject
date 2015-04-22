#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include "HashTable.h"
#include <vector>
#include "Networking.h"

using std::cerr; using std::endl; using std::cout;

int main(int argc, char*argv[]) {
  int server_fd, client_fd;
  struct sockaddr_storage their_addr;
  socklen_t size = sizeof(their_addr);
  
  server_fd = Socket(NULL, "9034", AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    cerr << "socket" << endl;
  if (listen(server_fd, 10) < 0)
    cerr << "listen" << endl;
  client_fd = accept(server_fd, (struct sockaddr *)&their_addr, &size);
  //cout << "legos" << endl;
  if (client_fd < 0) cerr << "client" << endl;;
  std::string s;
  for (int i = 0; i < 2999; ++i) {
    s += 'a';
  }
  int a;
  if ((a = Send(client_fd, s)) < s.length())
    cout << "send" << endl;
  else
    cout << a << endl;
  close(client_fd);
  close(server_fd);
}
