#ifndef PETER_DELEVORYAS_UTIL_H
#define PETER_DELEVORYAS_UTIL_H
#include <iostream>
extern "C"
{
#include <sys/types.h>
}

#define DEFAULTUSERNAME "anonymous"
#define POSTINTOPIC 'P'
#define POSTTOPIC 'N'
#define GETTOPIC 'T'
#define GETLISTTOPICS 'L'
//#define TITLELEN 20
//#define USERLEN 20
//#define POSTLEN 140
//#define MAXTOPICS 1000
#define TITLELEN 101
#define USERLEN 101
#define POSTLEN 1001
#define MAXTOPICS 100
#define MAXPOSTS 10 
#define TABLESIZE 3000

struct Post
{
  char username[USERLEN];
  char text[POSTLEN];
};

struct Topic
{
  char title[TITLELEN];
  //int nposts;
  Post posts[MAXPOSTS];
};

struct Request
{
  char type;
  Post post;
  Topic topic;
};

// Attempts to read n bytes from the given file descriptor,
// copying them to the buffer given by vptr. Returns
// the actual number of bytes read, or -1 on failure.
ssize_t readn(int fd, void *vptr, size_t n);
// Writes n bytes starting at vptr to the given file descriptor.
// Returns the number of bytes sucessfully written, or -1 on failure
ssize_t writen(int fd, const void *vptr, size_t n);
// Converts post to string of fields delimited by newlines
// 
std::string post_to_string(Post *post);
void print_post(Post *post);

#endif//PETER_DELEVORYAS_UTIL_H
