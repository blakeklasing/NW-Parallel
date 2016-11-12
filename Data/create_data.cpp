/*
Blake Klasing
Stacy Gramajo
Sequential Needleman-Wunsch algorithm
September 12, 2016
*/

#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void createData(int length, char alphabet[], string filename)
{
  ofstream file;
  
  file.open(filename);

  char c;
  srand(time(NULL));
  for (int i = 0; i < length; i++)
  {
    c = alphabet[rand() % 5];
    file << c;
  }

  file.close();
}

int main(int argc, char *argv[])
{
  char alphabet[] = { 'a', 'g', 'c', 'u', 't' };

  string filename = "some_data2_" + to_string(5000) + ".txt";

  createData(5000, alphabet, filename);
  
  return 0;
}
