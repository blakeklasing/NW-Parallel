/*
Parallel Program
Blake Kasing
Stacy Gramajo
COP 6616
Lock-Based Implementation 2
Needleman-Wunch Algorithm
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <climits>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define THREADS 4
#define MATCH 5
#define MISMATCH -1
#define GAP -2

struct Node
{
  int row,column;
  Node *next;
};

void enq(int i, int j, Node *&tail, Node *&head)
{
  //First Create the node
  Node *node = new Node;
  node->row = i;
  node->column = j;
  node->next = NULL;

  if(head == NULL)
  { //if it is the first one
    head = node;
    tail = node;
    return;
  }
  tail->next = node;
  tail = node;
}//end of enq function

Node* deq (Node *&head, Node *&tail)
{
    Node *first = head;
    if(head == NULL)
    { //nothing in the list
      return NULL;
    }
    else if(head->next == NULL)
    {
        head = NULL;
        tail = NULL;
    }
    else
    {
      Node *delNode = head;
      head = head->next;
    }
    return first;
}
void print(Node *head)//for debuggining
{
  if(head == NULL) //empty list
  {
    return;
  }
  Node* tmp = head;
  while(tmp != NULL)
  {
    //std::cout << "[" << tmp->row << "," << tmp->column << "]" << std::endl;
    if (tmp->next != NULL)
    {
      tmp = tmp->next;
    }
    else
    {
      tmp = NULL;
    }
  }
  std::cout << std::endl;
}
int maximum(int one, int two, int three)
{
  int maxNum = std::max(one, two);
  return std::max(maxNum, three);
}

void findvalue(std::string first, std::string second, std::vector<std::vector<int> > & grid, int i,
  int j, Node *&head, Node *&tail, std::mutex & enqueue)
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

      //add two other items from matrix
      if(grid.size() > (i+1) && grid[i+1][j] == INT_MIN)
      { //add and count++
        enqueue.lock();
        enq(i+1, j, tail, head);
        enqueue.unlock();
      }
      if(grid[0].size() > (j+1) && grid[i][j+1] == INT_MIN)
      {
        enqueue.lock();
        enq(i, j+1, tail, head);
        enqueue.unlock();
      }
    }
    else
    { //add the item back to the pool
      enqueue.lock();
      enq(i, j, tail, head);
      enqueue.unlock();
    }
  }
  void thread_function(std::vector<std::vector<int> > & grid, std::string first, std::string second,
    Node *&head, Node *&tail, std::mutex & enqueue, std::mutex & dequeue)
    {
      bool temp = false;
      //Remember to malloc the Node
      while(grid[grid.size()-1][grid[0].size()-1] == INT_MIN)
      {
        Node *item = (Node *) malloc (sizeof(Node));
        item->next = (Node *) malloc (sizeof(Node));
        item->next = NULL;

        dequeue.lock();
        if(head != NULL)
        {
          item = deq(head, tail);
          temp = true;
        }
        dequeue.unlock();

        if(temp)
        { //Calculate the matrix item
          findvalue(first, second, grid, item->row, item->column, head, tail, std::ref(enqueue));
          temp = false;
        }
        delete item;
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
      Node *head = NULL;
      Node *tail = NULL;
      std::mutex enqueue, dequeue;

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
        threads[i] = std::thread(thread_function, std::ref(grid), first, second, std::ref(head), std::ref(tail), std::ref(enqueue), std::ref(dequeue));
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
