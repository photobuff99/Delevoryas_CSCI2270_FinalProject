#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <time.h>

#define POSTINTOPIC 'P'
#define GETTOPIC 'G'
#define GETLISTTOPICS 'A'

struct Post
{
  time_t posted;
  char username[20];
  char text[140];
};

struct Topic
{
  time_t posted;
  char title[20];
  char username[20];
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
