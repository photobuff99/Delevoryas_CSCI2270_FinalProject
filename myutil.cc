#include "myutil.h"
#include <iostream>
#include <string>
#include <sstream>

extern "C"
{
#include <errno.h>
#include <unistd.h>
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = (const char *) vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;
      else
        return -1;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}

ssize_t readn(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = (char *) vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        return -1;
    } else if (nread == 0) {
      break;
    }
    nleft -= nread;
    ptr += nread;
  }
  return n - nleft;
}

std::string post_to_string(Post *post)
{
  std::string post_str;
  std::string line;
  int bytes_read;
  char c;

  bytes_read = 0;
  post_str += std::string(post->username);
  while (bytes_read < POSTLEN) {
    c = post->text[bytes_read];
    if (c != '\0') {
      line += c;
    } else {
      line += '\n';
      post_str += line;
      line = "";
    }
    bytes_read++;
  }

  return post_str;
}

void print_post(Post *post)
{
  std::stringstream ss(post_to_string(post));
  std::string line;

  std::getline(ss,line);
  std::cout << line << " posted: " << std::endl;
  while (std::getline(ss,line)) {
    if (line != "")
      std::cout << " " << line << std::endl;
  }
}
