#ifndef PETERDELEVORYAS_CHAT_UTIL_H
#define PETERDELEVORYAS_CHAT_UTIL_H
#include <sys/types.h>

#define POSTINTOPIC 'P'
#define GETTOPIC 'G'
#define GETLISTTOPICS 'A'

struct Post
{
  char username[20];
  char text[140];
};

struct Topic
{
  char title[20];
  struct Post posts[10];
};

struct Request
{
  char type;
  struct Post post;
  struct Topic topic;
};

ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readn(int fd, void *vptr, size_t n);

#endif//PETERDELEVORYAS_CHAT_UTIL_H
