#include "Hash.h"
#include "myutil.h"
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
  Hash *table = new Hash();
  table->print();

  delete table;
}
