#include "HashTable.h"
#include <iostream>
#include <cmath>

HashTable::HashTable()
{
  memset(table, 0, sizeof table);
  memset(titles, 0, sizeof titles);
  collisions = 0;
}

HashTable::~HashTable()
{
  for (int i = 0; i < TABLESIZE; ++i)
    if (table[i] != NULL)
      delete table[i];
}

// param should be a null terminated char array
int HashTable::HashIndex(const char *str)
{
  unsigned long r;
  unsigned long hash = 5381;
  int c;
  while ((c = *(str++)))
    hash = ((hash << 5) + hash) + c;
  return hash % TABLESIZE;
}

// returns number of collisions
int HashTable::HashInsert(const Topic &topic)
{
  int x, c;
  Node *n;

  c = 0;
  x = HashIndex(topic.title);
  if (table[x] == NULL) { // empty spot
    table[x] = new Node(topic);
    return c;
  } else { // a collision
    collisions++;
    c++;
    n = table[x];
    while (n->next != NULL) {
      c++;
      n = n->next;
    }
    n->next = new Node(topic);
    n->next->prev = n;
    return c;
  }
}

// returns 0 on succes, -1 if not found
int HashTable::HashDelete(const char *title)
{
  int x;
  Node *n;

  x = HashIndex(title);
  if (table[x] == NULL) { // not found!
    return -1;
  } else if (table[x]->next == NULL) { // no colsns
    delete table[x];
    table[x] = NULL;
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
      return 0;
    }
  }
}

Topic *HashTable::HashSearch(const char *title)
{
  int x;

  if (table[x] != NULL)
    return &(table[x]->topic);
  else
    return NULL;
}

char *HashTable::HashTitles()
{
  return &(titles[0][0]);
}

int HashTable::HashCollisions()
{
  return collisions;
}
