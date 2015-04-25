#ifndef PETERDELEVORYAS_HASHTABLE_H
#define PETERDELEVORYAS_HASHTABLE_H
#include <vector>
#include <string>

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ChatUtil.h"
}

#define TABLESIZE 100

struct Node
{
  Topic topic;
  Node *next;
  Node *prev;

  Node(const Topic &t) : topic(t), next(NULL), prev(NULL){}
};

// Key = topic.title, value = topic
// Used hash function from:
// Max hashtable size: 15 topics, higher and collisions too high
class HashTable
{
 private:
  Node *table[TABLESIZE];
  char titles[20][10];
  int collisions;
  
 public:
  HashTable();
 ~HashTable();
  int HashIndex(const char *str);
  int HashInsert(const Topic &topic);
  int HashDelete(const char *title);
  Topic *HashSearch(const char *title);
  char *HashTitles();
  int HashCollisions();
};

#endif//PETERDELEVORYAS_HASHTABLE_H
