#ifndef PETERDELEVORYAS_CHAT_UTIL_H
#define PETERDELEVORYAS_CHAT_UTIL_H
#include <sys/types.h>

#define POSTINTOPIC 'P'
#define GETTOPIC 'G'
#define GETLISTTOPICS 'A'
#define MAXPOSTS 10
#define TITLELENGTH 20
#define POSTLENGTH 140
#define USERNAMELENGTH 20
#define TABLESIZE 3000
#define MAXTOPICS 1000 


struct Post
{
  char username[USERNAMELENGTH];
  char text[POSTLENGTH];
};

struct Topic
{
  char title[TITLELENGTH];
  struct Post posts[MAXPOSTS];
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
