#include "Hash.h"
#include <iostream>
#include <cmath>
#include "myutil.h"

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

Hash::Hash() : collisions(0), ntopics(0)
{
  FILE *bin;
  struct stat buffer;
  Topic t;

  // Clear memory for safety
  memset(table, 0, sizeof table);
  // If table.bin in pwd, use it, else, forget about it
  if (stat("table.bin",&buffer) == 0) {
    bin = fopen("table.bin","rb");
    if (!bin)
      perror("fopen");
    while (!feof(bin)) {
      fread(&t, sizeof(struct Topic), 1, bin);
      if (insert(t))
        printf("hash: inserted topic: %s\n", t.title);
    }
    printf("hash: topics read from file: %d\n", ntopics);
    fclose(bin);
  }
}

Hash::~Hash()
{
  int s;

  printf("hash: writing to bfile...\n");
  s = writetobfile();
  if (s < 0)
    printf("hash: ...error writing to disk\n");
  else
    printf("hash: ...done\n");
  for (int i = 0; i < TABLESIZE; ++i)
    if (table[i] != NULL)
      delete table[i];
}

// param should be a null terminated char array
// This hash function was created by Dan Bernstein,
// and all credit goes to him. Implementation taken
// from "http://www.cse.yorku.ca/~oz/hash.html".
int Hash::djb2(const char *str)
{
  unsigned long r;
  unsigned long hash = 5381;
  int c;
  while ((c = *(str++)))
    hash = ((hash << 5) + hash) + c;
  return hash % TABLESIZE;
}

// returns -1 if topic already present or 0 otherwise
int Hash::insert(const Topic &topic)
{
  int x;
  Node *n;
  
  if (ntopics == MAXTOPICS) // too many topics!
    return -1;
  // add to hash table
  x = djb2(topic.title);
  if (table[x] == NULL) { // empty spot
    table[x] = new Node(topic);
    table[x]->prev = NULL;
    table[x]->next = NULL;
  } else { // a collision
    collisions++;
    n = table[x];
    while (n->next != NULL) {
      if (strcmp(topic.title,n->topic.title) == 0)
        return -1;
      n = n->next;
    }
    if (strcmp(topic.title,n->topic.title) == 0)
      return -1;
    n->next = new Node(topic);
    n->next->prev = n;
    n->next->next = NULL;
  }
  ntopics++;
  return 0;
}

// returns 0 on success, -1 if not found
int Hash::remove(const char *title)
{
  int x;
  Node *n;

  // remove from table
  x = djb2(title);
  if (table[x] == NULL) { // not found!
    return -1;
  } else if (table[x]->next == NULL) { // no colsns
    delete table[x];
    table[x] = NULL;
    ntopics--;
    return 0;
  } else { // oh great there's collisions
    n = table[x];
    while (n != NULL &&
           strcmp(n->topic.title, title) != 0) {
      n = n->next;
    }
    if (n == NULL) { // not found!
      return -1;
    } else {
      if (n->prev) // link previous elem to next
        n->prev->next = n->next;
      else
        table[x] = n->next;
      if (n->next) // link next elem to prev
        n->next->prev = n->prev;
      delete n;
      collisions--;
      ntopics--;
      return 0;
    }
  }
}

int Hash::get(const char *title, Topic *buffer)
{
  Node *n;
  int x;

  memset(buffer, 0, sizeof(struct Topic));
  x = djb2(title);
  if (table[x] != NULL) {
    n = table[x];
    while (n != NULL && strcmp(n->topic.title, title) != 0) {
      n = n->next;
    }
    if (n == NULL) // if still not found
      return -1;
    memcpy(buffer, &(n->topic), sizeof(struct Topic));
    return 0;
  } else { // If no topics inserted at that index, then topic is definitely not in table
    return -1;
  }
}

int Hash::getcollisions()
{
  return collisions;
}

void Hash::print()
{
  Node *n;
  char buffer[USERLEN+POSTLEN];

  for (int i = 0; i < TABLESIZE; ++i) {
    if ((n = table[i]) != NULL) {
      while (n != NULL) {
        printf("%d: %s\n", i, n->topic.title);
        for (int j = 0; j < MAXPOSTS; ++j) {
          print_post(&(n->topic.posts[j]));
        }
        n = n->next;
      }
    }
  }
  std::cout << "# topics: " << ntopics << std::endl;
  
}

// returns -1 if failure, otherwise 0 on success
int Hash::writetobfile()
{
  FILE *bin;
  Node *n;
  struct stat buffer;

  // Remove old, if not present, remove() returns -1 but doesn't crash
  if (stat("table.bin",&buffer) == 0)
    remove("table.bin");
  bin = fopen("table.bin", "wb");
  if (!bin) // failed to open file stream
    return -1;
  // next, we need to write the data from all the nodes
  // this is because the nodes are on the heap, not the stack
  for (int i = 0; i < TABLESIZE; ++i)
    if (table[i] != NULL) {
      n = table[i];
      while (n != NULL) {
        fwrite(&(n->topic), sizeof(struct Topic), 1, bin);
        n = n->next;
      }
    }
  // all done! That was easy, wasn't it?
  fclose(bin);
  return 0;
}

int Hash::getntopics()
{
  return ntopics;
}

// REQUIRED: buffer must be of size MAXTOPICS * TITLELEN to prevent overflow
// gettopics() clears the buffer and fills it with the titles of the topics
// currently in the hash table. If is necessary for the buffer to be a static
// size so that it is easy to send and receive across the server-client connection,
// so it is necessary that the buffer is always MAXTOPICS * TITLELEN bytes large,
// to prevent overflow.
void Hash::gettopics(char *buffer)
{
  Node *n;           // Node pointer to iterate through the entire hash table
  int bytes;         // number of bytes actually copied into the buffer at any point.
  int numtopics = 0; // used for debugging, numtopics should equal ntopics (class variable)

  bytes = 0; // intially, zero bytes have been read
  memset(buffer, 0, MAXTOPICS*TITLELEN); // clear buffer
  for (int i = 0; i < TABLESIZE; ++i) { // check possible location in the hash table
    n = table[i]; // n now points to the current hash table array element
    while (n != NULL) { // while loop necessary in case it's a linked list, i.e. there is a collision at this index
      if (bytes >= MAXTOPICS*TITLELEN) break; // Once the number of bytes read is greater than or equal to the buffer size, stop copying
      strncpy(&buffer[bytes],n->topic.title,TITLELEN); // copy the title stored in the topic to the effective end of the buffer
      ++numtopics; // for debugging
      bytes += TITLELEN; // Increment bytes read. Also represents index of beginning of null (empty) section of the buffer.
      n = n->next; // If not a linked list, n->next == NULL, will cause to break from while loop. Else, repeat procedure with all nodes in the list
    }
  }
  // Primarily for debugging, will not appear client-side, only server-side.
  printf("Found: %d, ntopics = %d\n", numtopics, ntopics);
}

// Inserts new post at the end of the posts[] array of the specified topic struct.
// Note: it is important that the post is inserted at the end. Printing the array
// of posts involves iterating from beginning to end, printing the oldest post
// first and the newest post last. As well, this is important for deleting posts.
// Old posts are not actually deleted: each time a post is inserted, the other posts
// are shifted forward in the array, and then the new post is copied to the last
// element of the array, overwriting the posts[0] element (if it was there to begin
// with)
int Hash::insert(const Post &post, std::string topic)
{
  int index;

  index = djb2(topic.c_str());
  Node *n = table[index];
  // Determine if topic is actually in the table at the index
  if (n == NULL) {
    return -1;
  } else { // Element present, check if linked list
    while (strcmp(n->topic.title, topic.c_str()) != 0) {
      n = n->next;
    }
  }
  if (n == NULL) // wasn't found in linked list
    return -1;

  Post *posts = n->topic.posts;
  for (int i = 0; i < MAXPOSTS-1; i++) {
    memcpy(&(posts[i]), &(posts[i+1]), sizeof(struct Post));
  }
  memcpy(&(posts[MAXPOSTS-1]), &post, sizeof(struct Post));
  return 0;
}
