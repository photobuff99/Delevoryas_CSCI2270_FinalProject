#ifndef PETER_DELEVORYAS_UTIL_H
#define PETER_DELEVORYAS_UTIL_H
#include <iostream>
extern "C"
{
#include <sys/types.h>
}

// Default username in case user doesn't specify
#define DEFAULTUSERNAME "anonymous"

// Characters to include in Request struct to represent request type
#define POSTINTOPIC 'P'
#define POSTTOPIC 'N'
#define GETTOPIC 'T'
#define GETLISTTOPICS 'L'

// Lengths of various fields in structs. These must be static in order
// to ensure static struct size, in order to ensure easy transmission
// across sockets as byte arrays.
#define TITLELEN 101
#define USERLEN 101
#define POSTLEN 2001
#define MAXTOPICS 100
#define MAXPOSTS 10 
#define TABLESIZE 3000

// Represents a user post: includes the user's username and their message text
struct Post
{
  char username[USERLEN];
  char text[POSTLEN];
};

// Includes a topic title as well as a number of posts
struct Topic
{
  char title[TITLELEN];
  Post posts[MAXPOSTS];
};

// Used as a buffer to send across socket
// TODO: Implement transmissions with smaller struct/three-part handshake
struct Request
{
  char type;
  Post post;
  Topic topic;
};

  /*  readn and writen bypass an allocated socket's buffer size.
      Normally, we use the system calls read() and write() to 
      read and write from sockets, similar to how we would from
      a file. read() and write() even include the number of bytes
      to attempt to read as a parameter...so what's the difference
      between read() and readn(), and write() and writen()?
      read() will attempt to read n bytes from the socket, but if
      the socket's buffer size is smaller than n, then it will simply
      transfer the socket's buffer size # of bytes from the socket
      buffer to the user's buffer rather than n. However, if there
      were still more bytes to read, they are still available!
      That is, if the sender wrote 1024 bytes to the socket, and
      the socket's buffer size is 256, and you tried to read 1000,
      you would have gotten 256, but there are still 766 bytes left
      that you can read from the socket. So, if you call read()
      3 more times, you could get all the information from the socket
      that was written there by the sender! This is where readn()
      and writen() come in. No matter what size your buffer is,
      readn() and writen() will continue to attempt to transfer
      bytes until 0 bytes are read (or written), meaning there 
      really aren't any more bytes to get from the socket.
  */

// Attempts to read n bytes from the given file descriptor,
// copying them to the buffer given by vptr. Returns
// the actual number of bytes read, or -1 on failure.
ssize_t readn(int fd, void *vptr, size_t n);

// Writes n bytes starting at vptr to the given file descriptor.
// Returns the number of bytes sucessfully written, or -1 on failure
ssize_t writen(int fd, const void *vptr, size_t n);

// Converts post to string of fields delimited by newlines
std::string post_to_string(Post *post);
// Prints post by first using post_to_string, then printing each line
// in a special format: first prints username, then prints message lines
// with a 1 space indent.
void print_post(Post *post);

#endif//PETER_DELEVORYAS_UTIL_H
