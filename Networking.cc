#include "Networking.h"

// Converts request to string header
std::string Request::ToString()
{
  std::string s;
  s += method + " " + resource + "\r\n";
  s += host   + " " + port     + "\r\n";
  if (method == "POST" || method == "PUT") {
    s += user_post + "\r\n";
  }
  s += "\r\n";
  return s;
}

// Converts response to string header, including resource
std::string Response::ToString()
{
  std::string s;
  s += status + " " + descrip + "\r\n";
  s += "length: " + std::to_string(resource_length) + "\r\n";
  s += "\r\n";
  if (resource_length > 0) {
    s += "\r\n";
    s += "topic: " + topic;
    const int n = messages.size();
    for (int i = 0; i < n; ++i) {
      s += messages[i] + "\r\n";
    }
  }
  return s;
}

int Socket(const char *ip_addr, const char *port, int domain, int type, int protocol)
{
  struct addrinfo hints, *results, *iter;
  int socket_fd, status;

  // Fill out known information
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = domain;
  hints.ai_socktype = type;
  hints.ai_protocol = protocol;
  if (ip_addr == NULL) {
    hints.ai_flags = AI_PASSIVE;  // If server, IP should be self
  }

  status = getaddrinfo(ip_addr, port, &hints, &results);
  if (status != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << '\n';
    return -1;
  }

  for (iter = results; iter != NULL; iter = iter->ai_next) {
    socket_fd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
    if (socket_fd == -1) {
      continue;
    }
    if (ip_addr == NULL) { // If retrieving a server socket, call bind()
      status = bind(socket_fd, iter->ai_addr, iter->ai_addrlen);
      if (status == -1) {   // Success binding socket to self's IP and Port
        close(socket_fd);
        continue;
      }
      int yes = 1;
      status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      if (status == -1) {
        std::cerr << "set sockopt" << std::endl;
        exit(1);
      }
    } else if (ip_addr != NULL) { // If client, not necessary to call bind()
      status = connect(socket_fd, iter->ai_addr, iter->ai_addrlen);
      if (status == -1) {
        close(socket_fd);
        continue;
      }
        // Success retrieving client socket file descriptor
    }
    break;
  } // for loop : linked list

  // If reached end of linked list, none of the sockets/IPs/ports were valid
  if (iter == NULL) {
    if (ip_addr == NULL) {
      std::cerr << "Could not allocate socket/bind socket to IP/port\n";
    } else {
      std::cerr << "Cout not allocate socket\n";
    }
    // free linked list
    freeaddrinfo(results);
    return -1;
  }

  // free linked list
  freeaddrinfo(results);
  return socket_fd;
} // Socket()

// Wraps send() to make it easier to use
int Send(int socket_fd, std::string str)
{
  // If string.c_str() is < 500 bytes, transmit in single send() call
  // Else, transmit in 500 byte chunks
  int num_bytes_sent;
  if (str.length() <= 500) { // transmits a (max) 500 byte char string, last char is null
    return send(socket_fd, str.c_str(), str.length(), 0);
  } else {
      int num_bytes = str.length() + 1;
      num_bytes_sent = 0;
      int packet_size = 500;
      char buffer[num_bytes];
      strcpy(buffer, str.c_str());
      for (int i = 0; i < (num_bytes-1) / 500; ++i) {
        num_bytes_sent += send(socket_fd, buffer+num_bytes_sent, packet_size, 0);
      }
      packet_size = (num_bytes-1) % 500;
      num_bytes_sent += send(socket_fd, buffer+num_bytes_sent, packet_size, 0);
    }

    if (num_bytes_sent != str.length()) {
      std::cerr << "Error, only " << num_bytes_sent << " sent out of " << str.length() << '\n';
    }
    return num_bytes_sent;
}
std::string Recv(int socket_fd, int num_bytes)
{
  // if <= 500 bytes, just use a single recv() call
  // else use multiple calls, each receiving 500 bytes
  // each 500 byte packet is a null terminated string,
  // so technically it's only 499 characters at most.
  if (num_bytes <= 500) {
    char buffer[num_bytes+1];
    memset(buffer, 0, sizeof(buffer));
    recv(socket_fd, buffer, sizeof(buffer), 0);
    buffer[num_bytes] = '\0';
    return std::string(buffer);
  } else {
    int num_bytes_received = 0;
    char buffer[num_bytes+1];
    buffer[num_bytes] = '\0';
    for (int i = 0; i < (num_bytes) / 500; ++i) {
      num_bytes_received += recv(socket_fd, buffer+num_bytes_received, 500, 0);
    }
    num_bytes_received += recv(socket_fd, buffer+num_bytes_received,
                               num_bytes % 500, 0);
    return std::string(buffer);
  }
}
