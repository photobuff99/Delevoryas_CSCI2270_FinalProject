#include "Hash.h"
#include <fstream>
#include <iostream>
#include <string>
extern "C" 
{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
}

#define TOPICTEST "Nice day today?"
#define USERNAMETEST "anonymous"
#define TEST "Yeah it's pretty nice out today!"

using namespace std;

int main()
{
  string title_username;
  string post;
  struct Topic testtopic;
  Hash testtable;
  ifstream poststrings("posts.txt");
  ifstream usernames("usernames.txt");
  char buffer[256];

  for (int i = 0; i < 10; ++i) {
    memset(&testtopic, 0, sizeof testtopic); // clear topic
    getline(usernames, title_username);      // get username into title_username
    memset(buffer, 0, 256); // clear buffer for username
    strcpy(buffer,title_username.c_str());   // copy username into buffer
    memcpy(testtopic.title,buffer,20);       // copy username into testtopic.title, it's ok bceause same limit length
    for (int j = 0; j < 10; ++j) {           // 10 posts per topic
      memset(buffer,0,256);                  // clear buffer to use username for actual username field
      strcpy(buffer,title_username.c_str()); // copy username into buffer for post username field
      memcpy(testtopic.posts[j].username,buffer,20);  // copy username from buffer into actual post field

      getline(poststrings, post);           // get a post string line
      memset(buffer, 0, 256);               // clear buffer for use
      strcpy(buffer,post.c_str());          // copy string into buffer
      memcpy(testtopic.posts[j].text,buffer,140); // copy string from buffer into posts.text field
      // insert finished topic struct into table
      testtable.insert(testtopic);
    }
  }
  testtable.print();// check if it worked correctly
}
  
