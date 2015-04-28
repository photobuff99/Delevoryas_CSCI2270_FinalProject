#include "myutil.h"
#include <iostream>
#include <string>
#include <sstream>

extern "C"
{
#include <errno.h>
#include <unistd.h>
}

// See myutil.h for documentation
ssize_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;     // difference between n and num bytes written in total
  ssize_t nwritten; // num bytes written after one write() call
  const char *ptr;  // points to location in vptr

  ptr = (const char *) vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      // If write() is interrupted (signfied by EINTR) then set nwritten to 0
      // so that nleft stays the same, but keep trying to write.
      // If errno != EINTR, then return -1, some other error must have
      // occurred and it is unlikely to be recovered from.
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

// See myutil.h for documentation
ssize_t readn(int fd, void *vptr, size_t n)
{
  size_t nleft;   // difference between n and num bytes read in total
  ssize_t nread;  // num bytes read after one read() call
  char *ptr;      // points to location in vptr

  ptr = (char *) vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nread = read(fd, ptr, nleft)) < 0) {
      // If read() is interrupted (signfied by EINTR) then set nread to 0
      // so that nleft stays the same, but keep trying to read.
      // If errno != EINTR, then return -1, some other error must have
      // occurred and it is unlikely to be recovered from.
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
  post_str += std::string(post->username) + '\n';
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
