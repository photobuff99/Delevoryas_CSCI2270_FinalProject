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

/*
Function prototype:
Hash::Hash();

Function description:
Constructor for Hash class. Initializes the number of collisions and topics to 0,
checks for binary file "table.bin" in current directory. If "table.bin" is present,
inserts Topics recorded in "table.bin" into hash table. Else, constructs table
without any topics. Prints the title of each Topic inserted, and prints the
number of topics read from "table.bin" at the end of the read-in process.

Example:
Hash *table;
table = new Hash();
...some code using the table...
delete table; // at the end of your program

Pre-conditions: Optional: "table.bin" is in the directory that you execute the program
                          using the constructor.
Post-conditions: Hash table will contain previous topics if "table.bin" is present,
                 otherwise empty. If dynamically allocated, must be explicitly deleted.
*/
Hash::Hash() : collisions(0), ntopics(0)
{
  FILE *bin;
  struct stat buffer;
  Topic t;

  // Clear memory for safety
  memset(table, 0, sizeof table);
  // If table.bin in pwd, use it, else, forget about it
  if (stat("table.bin",&buffer) == 0) {
    printf("hash: found table.bin: attempting to reconstruct table\n");
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

/*
Function prototype:
Hash::~Hash();

Function description:
Removes "table.bin" if "table.bin" is present in current directory,
writes all Topic elements in table to binary file "table.bin" if
possible, then deletes Topic elements.

Example:
Hash *table;
table = new Hash();
...at the end of your program...
delete table; // calls destructor

Pre-conditions: None
Post-conditions: Table is written to binary file "table.bin" in current directory.
                 Allocated memory from table is freed.
*/
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

/*
Function prototype:
int Hash::djb2(const char *);

Function description:
Hash function created by Dan J. Bernstein, source: "http://www.cse.yorku.ca/~oz/hash.html".
ALL CREDIT FOR THIS FUNCTION GOES TO DAN J. BERNSTEIN.
This function takes a string parameter (the key for the table element,
in this program, the title of the Topic being inserted is used) and generates
a hash value, then the hash values is turned into an array index using
the modulus function and the size of the array being used to store Topics.

Example:
Node *table[TABLESIZE]; // the hash table's array
Topic t;                // a topic
t.title = "Topic title";// the topic must have a title
int hash_table_index = djb2(title); // get the index to insert the topic at
table[hash_table_index] = new Node(t); // insert the new topic at the generated index in the table

Pre-conditions: char* parameter is a null terminated character array.
Post-conditions: Returns the index that the topic should be inserted at, 0 <= index < TABLESIZE.
*/
int Hash::djb2(const char *str)
{
  unsigned long r;
  unsigned long hash = 5381;
  int c;
  while ((c = *(str++)))
    hash = ((hash << 5) + hash) + c;
  return hash % TABLESIZE;
}

/*
Function prototype:
int Hash::insert(const Topic&);

Function description:
Inserts the given Topic struct into the hash table using the hash function djb2() (see above).
First calculates hash index, then checks to see if there are any other Topics inserted at that
index in the table's array of Topics. If there are topics already inserted there, checks to make
sure that a duplicate topic (same title) is not there: if a duplicate topic is present, returns -1.
If it's not a duplicate topic, dynamically allocated space for a Node struct, which contains the
Topic struct being inserted, and then adds to the end of the linked list at that index.

Example:
Hash table;
Topic t;
t.title = "Topic title";
table.insert(t);
// a copy of t has been inserted into the table

Pre-conditions: Topic's title is not already in the hash table. Title is a null terminated character array.
Post-conditions: Returns -1 if a Topic with the same title is in the table. Otherwise, returns 0;
*/
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
/*
Function prototype:
int Hash::remove(const char*);

Function description:
This function attempts to find and remove the Topic in the hash table specified by
the title parameter. If the topic is not found, returns -1. If removal is successful,
returns 0.

Example:
Hash table;
Topic t;
t.title = "Topic title";
table.insert(t);
// Now I want to remove Topic t
if (table.remove("Topic title") != -1)
  cout << "Table successfully removed " << "Topic title" << endl;
else
  cout << "Table could not find " << "Topic title" << " in the hash table" << endl;

Pre-conditions: Optional: A topic with the specified title is in the hash table.
Post-conditions: Returns -1 if topic not found, 0 if removal is successful. If successful,
                 Topic's dynamically allocated space is freed.
*/
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

/*
Function prototype:
int Hash::get(const char*, Topic*);

Function description:
This function returns an integer value representing the success of finding
the topic specified by the title in the const char* parameter. If successful,
Topic* will point to a Topic containing the requested topic.

Example:
Hash table;
Topic t;
t.title = "Topic title";
Topic buffer;
memset(&buffer, 0, sizeof buffer); // clear buffer to ensure safety of return value
if (table.get("Topic title", &buffer) != -1)
  cout << "buffer is now equivalent to a copy of the Topic in the hash table" << endl;
else
  cout << "Topic with title specified is not present in hash table, buffer's values are undefined" << endl;

Pre-conditions: char* is a null terminated character array specifying the title of the topic to be retrieved,
                and Topic* is a pointer to a Topic struct whose memory has already been allocated.
Post-conditions: If returns -1, Topic*'s values are undefined, and the Topic was not found in the hash table.
                 If returns 0, Topic* is equivalent to a copy of the Topic found in the hash table,
                 but does not point to the topic that is actually in the hash table.
*/
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

/*
Function prototype:
int Hash::getcollisions();

Function description:
Used for diagnostic purposes to query the hash table about how many collisions it holds.
I.E., basic getter method for collisions variable.

Example:
Hash table;
Topic t, v;
t.title = "Topic title";
v.title = "Topic title which generates same hash index as t's title";
cout << "There are " << table.getcollisions() << " collisions occuring in the hash table" << endl; // This will print "1 collision occurring"

Pre-conditions: None
Post-conditions: Returns the number of collisions currently occuring in the hash table, i.e. how many
                 items in the table have to share an index with another Topic through a linked list.
*/
int Hash::getcollisions()
{
  return collisions;
}

/*
Function prototype:
void Hash::print();

Function description:
This function prints the contents of the hash table at the time it is called.
Format:
IndexInsertedAt: TopicTitle2
UserName1 posted:
 message message message
 message message message
UserName2 posted:
 message message message
 message message message
IndexInsertedAt: TopicTitle2
UserName3 posted:
 message message message
 message message message
 message message message
 message message message

Example:
Hash table;
table.print(); // will print nothing, since nothing has been inserted

Pre-conditions: None
Post-conditions: Hash table is printed in specified format to stdout.
*/
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
/*
Function prototype:
int Hash::writetobfile();

Function description:
This function takes the current topics in the table and writes them to
a binary file called "table.bin". If "table.bin" already exists in the
current directory, it is removed before being rewritten. The "table.bin"
file created can be used the next time the table's constructor is called
to rebuild the table as it was before the destructor was called.
This function is used in the destructor of the Hash class to save the state
of the table.

Example:
Hash *table;
table = new Hash();
table.writetobfile(); // Useful if you want to save the table data, in case the program crashes before
                      // the destructor is called
...at the end of the program...
delete table; // This will call writetobfile() again

Pre-conditions: None
Post-conditions: If "table.bin" is in current directory, it is removed. Then, whether
                 or not "table.bin" was removed, a new "table.bin" file is written,
                 containing the Topic structs in the hash table at the time writetobfile()
                 is called.
*/
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

// Returns the number of topics currently in the hash table.
int Hash::getntopics()
{
  return ntopics;
}

/*
Function prototype:
void Hash::gettopics(char*);

Function description:
This function fills the buffer specified by char* with the titles of the Topics currently
in the hash table. Each Topic title takes up TITLELEN bytes, no matter what, and has
a null terminator at the end of the title. Buffer is statically allocated, so its
size must be MAXTOPICS * TITLELEN bytes.

Example:
Hash table;
Topic t;
t.title = "Topic title";
table.insert(t);
char buffer[MAXTOPICS * TITLELEN];
table.gettopics(buffer);
for (int i = 0; i < MAXTOPICS; ++i)
  if (buffer[i*TITLELEN] != '\0')
    cout << "Topic title " << i << ": " << buffer+(i*TITLELEN) << endl; // Prints each topic title on a separate line

Pre-conditions: char* buffer must be MAXTOPICS*TITLELEN bytes long, otherwise it will not be capable of holding all the
                topics in the hash table.
Post-conditions: The buffer will contain as many Topic titles as were found in the hash table. They will not necessarily be
                 one after the other in the buffer: There may be empty title spaces in between titles. Empty spaces will be
                 set to '\0'.
*/
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
/*
Function prototype:
int Hash::insert(const Post&, std::string);

Function description:
This function inserts a post into the Topic specified by the string parameter.
If the topic is not found in the hash table, -1 is returned. If the topic is found,
the posts in that topic are shifted forwards in their array, and the newest post
(the one being inserted) is copied to the last element of the array. The new
post is always inserted at the end of the array, even if there aren't any posts
in the other spots of the array. This is to make printing the list of posts from
oldest to newest easier.

Example:
Hash table;
Topic t;
t.title = "Topic title";
table.insert(t);
Post p;
p.username = "randomusername";
p.text = "Message from randomusername";
if (table.insert(p, "Topic title") != -1)
  cout << "Post from " << p.username << " successfully inserted into topic" << endl;
else
  cout << "Topic specified was not found, post was not inserted" << endl;

Pre-conditions: The Topic specified by topic should be in the hash table, however it
                will not crash the function if it is not.
Post-conditions: Returns -1 if topic not found, 0 if post is successfully copied into
                 the specified topic. The Topic will now contain the post specified by
                 post, and if there were MAXPOSTS posts in that topic, the oldest one
                 will be erased (overwritten). The newest post will be inserted at the
                 end of the post array in the specified topic, and the rest of the topics
                 will be shifted forward in the array.
*/
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
