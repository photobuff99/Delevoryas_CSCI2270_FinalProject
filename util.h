#ifndef PETER_DELEVORYAS_UTIL_H
#define PETER_DELEVORYAS_UTIL_H
#include <sys/types.h>

#define POSTINTOPIC 'P'
#define POSTTOPIC 'N'
#define GETTOPIC 'T'
#define GETLISTTOPICS 'L'
#define TITLELEN 20
#define USERLEN 20
#define POSTLEN 140
#define MAXPOSTS 10 
#define MAXTOPICS 1000 
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
  struct Post posts[MAXPOSTS];
};

struct Request
{
  char type;
  struct Post post;
  struct Topic topic;
};

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);

#endif//PETER_DELEVORYAS_UTIL_H
