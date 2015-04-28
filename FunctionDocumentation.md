# Function Documentation

# Hash Table Class (Hash.h, Hash.cc)
* Hash()
  * Hash table constructor: if binary file "table.bin" present in current directory, attempts to construct table from file, else constructs empty table.
* ~Hash()
  * Hash table destructor: if binary file "table.bin" present in current directory, deletes "table.bin", then writes current state of hash table to binary file, then destroys allocated space.
* int djb2(const char *str)
  * Dan J. Bernstein's hash algorithm: All credit goes to him. Source: http://www.cse.yorku.ca/~oz/hash.html
* int insert(const Topic &topic)
  * Attempts to insert topic in table using djb2
    * If there is another topic in the table with same title as the given topic, returns -1
    * If MAXTOPICS has been reached, returns -1
    * Else, returns 0
  * If collision, constructs linked list at index
* int insert(const Post &post, std::string topic)
  * Attempts to insert post within the topic in the table
    * If the topic is not in the table, returns -1
    * Else, returns 0
  * If MAXPOSTS has been reached, overwrites oldest post in topic
* int remove(const char *title)
  * Attempts to remove the topic with the matching title
    * If topic not present, returns -1
    * Else, returns 0
* int get(const char *title, Topic *buffer)
  * Attempts to copy the topic given by title into the buffer topic
    * If topic not present, returns -1, buffer is memset() to NULL characters
    * Else, returns 0, topic is copied into buffer using memcpy()
* void print()
  * Prints the current topics in the table
    * Iterates through all possible indexes of the hashtable
    * If index is valid, prints topic title and list of posts
* int writetobfile()
  * If "table.bin" is in current directory, removes "table.bin"
  * Writes topic structs to binary file
  * Returns -1 if unable to open "table.bin" filestream
  * Otherwise, returns 0
* void gettopics(char *buffer)
  * buffer is expected to be MAXTOPICS*TITLELEN bytes (at least)
  * Clears buffer (memset() to NULL characters)
  * Iterates through table, for every topic, add title to buffer
  * If no topics, all characters will be NULL in buffer
  * Else, each title will take up TITLELEN bytes in the array
  * Each added title is immediately after the other in the array
  * Even if a title's length < TITLELEN-1 bytes, all TITLELEN bytes are used.
  * Extra space is padded with NULL characters
