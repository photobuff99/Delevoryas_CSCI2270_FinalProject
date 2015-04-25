#ifndef PETERDELEVORYAS_HASH_H
#define PETERDELEVORYAS_HASH_H
#include <vector>
#include <string>

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ChatUtil.h"
}

#define TABLESIZE 3000
#define MAXTOPICS 800 
#define TITLELENGTH 20

struct Node
{
  Topic topic;
  Node *next;
  Node *prev;
  Node() : next(NULL), prev(NULL){}
  Node(const Topic &t) : topic(t), next(NULL), prev(NULL){}
};

// Key = topic.title, value = topic
// Used hash function from:
// Max hashtable size: 15 topics, higher and collisions too high
class Hash
{
 private:
  Node *table[TABLESIZE];
  int collisions;
  int ntopics;
  
 public:
  Hash();
 ~Hash();
  int djb2(const char *str);
  int insert(const Topic &topic);
  int remove(const char *title);
  Topic *get(const char *title);
  int getcollisions();
  void print();
  int writetobfile();
  int getntopics();
  void gettopics(char *buffer); // MUST BE MAXTOPICS*TITLELENGTH longs
};

#endif//PETERDELEVORYAS_HASH_H
