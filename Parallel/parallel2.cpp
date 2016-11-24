/*
Parallel Program
Black Kasing
Stacy Gramajo
COP 6616
Lock-free Implementation
Needleman-Wunch Algorithm
*/
#include <iostream>
#include <thread>
#include <vector>
#include <climits>
#include <fstream>
#include <stdio.h>
#include <atomic>
#include <time.h>
#include <string.h>
#define THREADS 4
#define MATCH 5
#define MISMATCH -1
#define GAP -2

struct Node
{
  int row,column;
  std::atomic<Node*> *next;
};

void enq(int i, int j, Node *tail, Node *head)
{
  //First Create the node
  Node *node = (Node *) malloc (sizeof(Node));
  node->row = i;
  node->column = j;
  node->next = (std::atomic<Node*> *) malloc (sizeof(std::atomic<Node*>));
  node->next->store(NULL);
  if(head->next->load() == NULL)
  { //if it is the first one
    bool tempBool;
    Node *tempNode = tail->next->load();
    tempBool = head->next->compare_exchange_strong(tempNode, node);
    if(tempBool)
    {
      tail->next->compare_exchange_strong(tempNode, node);
      return; //head is changed
    }
  }
  while(true)
  {
    Node *last = tail->next->load();
    Node *next = last->next->load();
    if(last == tail->next->load())
    {
      if(next == NULL)
      {
        if(last->next->compare_exchange_strong(next, node))
        {
          tail->next->compare_exchange_strong(last, node);
          return;
        }
      }
      else
      {
        tail->next->compare_exchange_strong(last, next);
      }
    }//first if stmt
  }//end of while loop
}//end of enq function
Node* deq (Node *head, Node *tail)
{
  while(true)
  {
    if(head->next->load() == NULL)
    { //nothing in the list
      return NULL;
    }
    Node *first = head->next->load();
    Node *last = tail->next->load();
    Node *next = first->next->load();
    if(first == head->next->load())
    {
      if (first == last)
      { //then it will be Null
        tail->next->compare_exchange_strong(last, NULL);
        //return;
      }
      else
      {
        if(head->next->compare_exchange_strong(first, next))
        {
          return first;
        }
      }
    }
  }
}
void print(Node *head)//for debuggining
{
  if(head->next == NULL) //empty list
  {
    return;
  }
  Node* tmp = head->next->load();
  while(tmp != NULL)
  {
    std::cout << "[" << tmp->row << "," << tmp->column << "]" << std::endl;
    if (tmp->next != NULL)
    {
      tmp = tmp->next->load();
    }
    else
    {
      tmp = NULL;
    }
  }
}
int maximum(int one, int two, int three)
{
  int maxNum = std::max(one, two);
  return std::max(maxNum, three);
}

void findvalue(std::string first, std::string second, std::vector<std::vector<int> > & grid, int i,
  int j, Node *head, Node *tail)
  { //normal calculation that can be seen in the NW sequential program
    if(grid[i][j] != INT_MIN)
    {//already been calculated or its being calculated
      return;
    }

    if(grid[i-1][j] != INT_MIN && grid[i-1][j-1] != INT_MIN && grid[i][j-1] != INT_MIN)
    {
      grid[i][j] = 0;
      bool match = (second.at(i-1) == first.at(j-1))? true: false;
      int diag = grid[i-1][j-1] + (match? MATCH:MISMATCH);
      int up = grid[i-1][j] + GAP;
      int left = grid[i][j-1] + GAP;
      grid[i][j] = maximum(diag, up, left);
      //std::cout << "[" << i << "," << j<< "]" << std::endl;

      //add two other items from matrix
      if(grid.size() > (i+1) && grid[i+1][j] == INT_MIN)
      { //add and count++
        enq(i+1, j, tail, head);
      }
      if(grid[0].size() > (j+1) && grid[i][j+1] == INT_MIN)
      {
        enq(i, j+1, tail, head);
      }
    }
    else
    { //add the item back to the pool
      enq(i, j, tail, head);
    }
  }
  void thread_function(std::vector<std::vector<int> > & grid, std::string first, std::string second,
    Node *head, Node *tail)
    {
      bool temp = false;
      //Remember to malloc the Node
      Node *item = (Node *) malloc (sizeof(Node));
      item->next = (std::atomic<Node*> *) malloc (sizeof(std::atomic<Node*>));
      item->next->store(NULL);
      while(grid[grid.size()-1][grid[0].size()-1] == INT_MIN)
      {
        if(head->next != NULL)
        {
          item = deq(head, tail);
          if(item != NULL)
          {
            temp = true;
          }
        }

        if(temp)
        { //Calculate the matrix item
          findvalue(first, second, grid, item->row, item->column, head, tail);
          temp = false;
        }
      }//end while loop
    }

    void fillInMatrix(std::vector<std::vector<int> > & grid, std::string first, std::string second)
    {
      //An extra row and column was added for the gap score
      int strlength1 = first.length() + 1;
      int strlength2 = second.length() + 1;

      for(int k = 0; k < strlength2; ++k)
      {
        grid[k][0] = GAP * k;
      }

      //Fill in the first column
      for(int j = 0; j < strlength1; ++j)
      {
        grid[0][j] = GAP * j;
      }
    }

    int main(int argc, char *argv[])
    {
      //Set up the variables
      std::string first = "AGGACCGCACAACCTTGCAGCTCAGCGACTCGTGGGGTCACACACACTGGCTGTCCAGCACCCTCTTTGGTGCCTTCACTTCCTTCCTTACCCTGAGGACCCACGGCCCATCGTTACAACTGCTTCCTTGCGTGCACTGCCAACCTTCCTTCCTTTCCAGCTCCCTGACTCTCCCCACCCACACAGGAGCAGAAGCGGGCTCCCCCACCTGCTCTCCCCGAGTCACCCCCATTCATCACACAGCGCCATCCGTCAGGAGATTCTCTCCAGCCTCACCTACGGGCAAGTCCTATCAGCCCCATCTTCACAGTGTCCACTTCTGGACACACACACACACACACACACACACACACACACGGTCCGCTACAGCTGCTTTTGGTAAGGAAATCGAATCAGGTCGTGCCCCTTGCTCTGCGCCCTCCGGCGCCTTCCCAATCCACTCCCAGTGAGATCAAGTTTCCCGCCGTAGGCATTAAAGTCTCCCTGATTCTCCAGCCTCACCTCCTGCCATTCTCCATGCTGACCCTCTCCTTGTTCCTTAAATACACCTGCCTTGTCTGACTCAGCCTGGGTACCTGTTGTTCCTTCCGTCTCCATGTGTGCGTGTGTGCACGTATGCTATATCTTGCTGCTCTGCTCCATCCAGCTGTCACCTCTCCAGAGAGGCCTTCCCTAGCCATCCTCACCCCCTACTCTTTCACTTCGCCTTATCTGCCCTCCCTTGTGCTTCTGGCTCTCTGACTACACACGAGATGTTTGTTCTCTGCTCTGTGTCTCTGCCCACTGCTGGAGCACCACGACTTTGACTTGTTCTCTACGACCTAGTGCTCAGAAAAGCGCTCCTGCACATAGAAGGCCCTGAATAAATATTTGTTGAATGAATGAACAGCCACCACCGTGGCCATAAGAGGTTAGGCTGTTCACAGCTCCCAGGGCTGCTGGCGGCCCCGTGGAGGAGCGCAGTGGACAGTTTC";
      std::string second = "GATCACACATAACTGTGGTGTCATGCATTTGGTATCTTTTAATTTTTAGGGGGGGAACTTGCTATGACTCAGCTATGACCGTAAAGGTCTCGTCGCAGTCAAATAATTTGTAGCTGGGCTTATTTATCTTTCACGGGTCGGGCATAGATACCCATAAGGGGTTACTCAGTCAATGGTCGCAGGACATACAGTACATAAACGCCACTAAATCGAACGAACGACGCACGCGTACGCATACGTACGCATACGTACGCATACGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCAAGTGTACGGAAGTGTAAGGGTAAGCAAGGGTTTTTAGATTTTAAACTAGCTTAATCAAACCCCCCTTACCCCCCGTAACTTCAAGGAGCTTATGCACATTTATGGATGTCTTGCCAAACCCCCAAAACCAGGATTAATGGACATGCAAACCTGAAGGTTTTACACCCCAAATCCCACTAATAAGTAAAATTCCAGCCGA";
      Node *head = (Node *) malloc (sizeof(Node));
      head->next = (std::atomic<Node*> *) malloc (sizeof(std::atomic<Node*>));
      head->next->store(NULL);

      Node *tail = (Node *) malloc (sizeof(Node));
      tail->next = (std::atomic<Node*> *) malloc (sizeof(std::atomic<Node*>));
      tail->next->store(NULL);

      /*deq(head, tail);
      enq(1, 1, tail, head);
      deq(head, tail);
      //std::cout << "Deleted Item: " << "[" << temp->row << "," << temp->column << "]" << std::endl;
      enq(1, 2, tail, head);
      enq(1, 3, tail, head);
      //deq(head, tail);
      print(head);*/

      std::cout << "First String Length: " << first.length() << std::endl;
      std::cout << "Second String Length: " << second.length() << std::endl;
      //Setting up the matrix
      std::vector<std::vector<int> > grid((second.length()+1), std::vector<int>((first.length()+1), INT_MIN));

      std::vector<std::thread> threads(THREADS);
      fillInMatrix(std::ref(grid), first, second);
      //create the pool of squares from the matrix
      enq(1, 1, tail, head);

      //Setting up the threads
      for (int i = 0; i < THREADS; ++i)
      {
        threads[i] = std::thread(thread_function, std::ref(grid), first, second, head, tail);
      }
      clock_t begin_time = clock();
      //Run the threads
      for (int i = 0; i < THREADS; ++i)
      {
        if (threads[i].joinable())
        {
          threads[i].join();
        }
      }
      std::cout << "Score: " << grid[grid.size()-1][grid[0].size()-1] << std::endl;
      // Check the time
      clock_t end_time = clock();
      float diffticks = end_time - begin_time;
      float diffms = (diffticks) / CLOCKS_PER_SEC;
      std::cout << "Time Elapse: " << diffms << std::endl;
    }
