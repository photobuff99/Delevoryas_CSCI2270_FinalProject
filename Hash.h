#ifndef PETERDELEVORYAS_HASH_H
#define PETERDELEVORYAS_HASH_H
#include <vector>
#include <string>
#include "myutil.h"
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
}

// Used to implement linked list chain
struct Node
{
  Topic topic;
  Node *next;
  Node *prev;
  Node() : next(NULL), prev(NULL){}
  Node(const Topic &t) : topic(t), next(NULL), prev(NULL){}
};

// For the Hash Table:
// Key = topic.title, value = topic
class Hash
{
 private:
  Node *table[TABLESIZE]; // see myutil.h for TABLESIZE definition
  int collisions;         // tracks number of collisions
  int ntopics;            // tracks number of topics in table
  
 public:
  // Constructs table from table.bin, if binary file not in
  // current directory, creates empty hash table.
  Hash();

  // Writes current topics to table.bin.
 ~Hash();
  
  // Hash function created by Dan J. Bernstein, source: http://www.cse.yorku.ca/~oz/hash.html
  int djb2(const char *str);

  // Inserts a topic into the hash table, returns 0 on success, -1 otherwise
  // Only inserts topic if the same title key has not been used.
  // If same title of topic is in the table, returns -1
  int insert(const Topic &topic);

  // Inserts a post into the hash table in the specified topic,
  // returns 0 on success, -1 otherwise. If topic is not actually
  // in the hash table, returns -1.
  int insert(const Post &post, std::string topic);

  // Removes the specified topic from the table
  // Returns -1 if not found, 0 on success
  int remove(const char *title);

  // Copies the specified topic, identified by title, into the buffer.
  // Returns -1 on failure, 0 on success. If topic not in table,
  // returns -1.
  int get(const char *title, Topic *buffer);

  // Returns collisions variable
  int getcollisions();

  // Prints the current state of the has table, starting at the beginning of the table
  // (i.e., index = 0). Prints collisions in the order they were inserted.
  // Prints each topic as the title, and then a series of posts, each containg a username
  // and the text to accompany it.
  void print();

  // Writes each topic struct to a binary file, in order of smallest to largest index
  // in table. Deletes old bfile, if present in current directory.
  // Returns -1 on failue to write table, 0 on success
  int writetobfile();

  // Returns ntopics variable.
  int getntopics();

  // Fills the buffer with the titles of all the topics currently in
  // the table. Buffer is expected to by MAXTOPICS*TITLELEN long.
  // Each topic title is TITLELEN bytes long, each topic title will
  // be added TITLELEN bytes at a time, even if the actual length of 
  // the title is less than TITLELEN-1 characters long
  void gettopics(char *buffer);
};

#endif//PETERDELEVORYAS_HASH_H
