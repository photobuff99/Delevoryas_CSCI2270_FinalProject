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

#define teststr "aaaaaaaaaaaaaaaaaaa"

using namespace std;

int main()
{
  Hash h;
  char name[20] = teststr;
  char text[140];
  Post p;
  Topic t;

  for (int i = 0; i < 140; ++i)
    text[i] = 'b';

  strcpy(p.username, name);
  strcpy(p.text, text);
  strcpy(t.title, name);
  for (int i = 0; i < 10; ++i) {
    strcpy(t.posts[i].username, name);
    strcpy(t.posts[i].text, text);
  }
  h.insert(t);
  h.remove(t.title);

  FILE *fp = fopen("RandomStrings.txt", "r");
  char str[11];
  for (int i = 0; i < 1000; ++i) {
    fread(t.title, 11, 1, fp);
    t.title[10] = '\0';
    h.insert(t);
  }
  fclose(fp);
  fp = fopen("RandomStrings.txt", "r");
  for (int i = 0; i < 1000; ++i) {
    if (i == 500)
    fread(t.title, 11, 1, fp);
    t.title[10] = '\0';
  }
  fclose(fp);
  //cout << h.HashCollisions() << '\n';
  //h.print();
  char buffer[TITLELENGTH*MAXTOPICS];
  h.gettopics(buffer);
  printf("num collisions %d\n", h.getcollisions());
  for (int i = 0; i < MAXTOPICS; ++i)
    printf("%d: %s\n",i, &buffer[i*TITLELENGTH]);
}
