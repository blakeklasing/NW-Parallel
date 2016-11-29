/*
    Parallel Program
    COP 6616
    Lock-Based Implementation 1
    Needleman-Wunch Algorithm
*/
#include <iostream>
#include <thread>
#include <mutex>
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

class poolItem
{
  public:
    int row, column;
    void addItem(int, int);
    poolItem() {}
    ~poolItem(){}
};

void poolItem::addItem(int i, int j)
{
    row = i;
    column = j;
}

int maximum(int one, int two, int three)
{
    int maxNum = std::max(one, two);
    return std::max(maxNum, three);
}

void findvalue(std::string first, std::string second, std::vector<std::vector<int> > & grid, int i,
               int j, std::vector<poolItem> * pool, std::mutex & dequeue)
{ //normal calculation that can be seen in the NW sequential program
    poolItem item1;
    poolItem item2;
    if(grid[i][j] != INT_MIN)
    {//already been calculated
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
    //  std::cout << "[" << i << "," << j<< "]" << std::endl;

      //add two other items from matrix
      if(grid.size() > (i+1) && grid[i+1][j] == INT_MIN)
      { //add and count++
        item1.addItem(i+1, j);
        dequeue.lock();
        pool->push_back(item1);
        dequeue.unlock();
      }
      if(grid[0].size() > (j+1) && grid[i][j+1] == INT_MIN)
      {
          item2.addItem(i, j+1);
          dequeue.lock();
          pool->push_back(item2);
          dequeue.unlock();
      }
    }
    else
    { //add the item back to the pool
      item1.addItem(i, j);
      pool->push_back(item1);
    }
}
void thread_function(std::vector<std::vector<int> > & grid, std::string first, std::string second, long & count,
                     std::mutex & enqueue, std::vector<poolItem> * pool, std::mutex & stmt, std::mutex & dequeue)
{
    bool temp = false;
    poolItem item;
    while(grid[grid.size()-1][grid[0].size()-1] == INT_MIN)
    {
        //need to lock the pool because another thread might retrieve the item
        dequeue.lock();
        if(!pool->empty())
        {
            item = pool->front();
            pool->erase(pool->begin()); //erase first item from the vector
            temp = true;
        }
        dequeue.unlock();

        if(temp)
        { //Calculate the matrix item
          findvalue(first, second, grid, item.row, item.column, pool, std::ref(dequeue));
          temp = false;
        }

    }
}

void fillInMatrix(std::vector<std::vector<int> > & grid, std::string first, std::string second)
{
  /*An extra row and column was added for the gap score*/
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
  std::string first = "GATCACACATAACTGTGGTGTCATGCATTTGGTATCTTTTAATTTTTAGGGGGGGAACTTGCTATGACTCAGCTATGACCGTAAAGGTCTCGTCGCAGTCAAATAATTTGTAGCTGGGCTTATTTATCTTTCACGGGTCGGGCATAGATACCCATAAGGGGTTACTCAGTCAATGGTCGCAGGACATACAGTACATAAACGCCACTAAATCGAACGAACGACGCACGCGTACGCATACGTACGCATACGTACGCATACGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCACGTGTACGCAAGTGTACGGAAGTGTAAGGGTAAGCAAGGGTTTTTAGATTTTAAACTAGCTTAATCAAACCCCCCTTACCCCCCGTAACTTCAAGGAGCTTATGCACATTTATGGATGTCTTGCCAAACCCCCAAAACCAGGATTAATGGACATGCAAACCTGAAGGTTTTACACCCCAAATCCCACTAATAAGTAAAATTCCAGCCGA";
  std::string second = "AGGACCGCACAACCTTGCAGCTCAGCGACTCGTGGGGTCACACACACTGGCTGTCCAGCACCCTCTTTGGTGCCTTCACTTCCTTCCTTACCCTGAGGACCCACGGCCCATCGTTACAACTGCTTCCTTGCGTGCACTGCCAACCTTCCTTCCTTTCCAGCTCCCTGACTCTCCCCACCCACACAGGAGCAGAAGCGGGCTCCCCCACCTGCTCTCCCCGAGTCACCCCCATTCATCACACAGCGCCATCCGTCAGGAGATTCTCTCCAGCCTCACCTACGGGCAAGTCCTATCAGCCCCATCTTCACAGTGTCCACTTCTGGACACACACACACACACACACACACACACACACACGGTCCGCTACAGCTGCTTTTGGTAAGGAAATCGAATCAGGTCGTGCCCCTTGCTCTGCGCCCTCCGGCGCCTTCCCAATCCACTCCCAGTGAGATCAAGTTTCCCGCCGTAGGCATTAAAGTCTCCCTGATTCTCCAGCCTCACCTCCTGCCATTCTCCATGCTGACCCTCTCCTTGTTCCTTAAATACACCTGCCTTGTCTGACTCAGCCTGGGTACCTGTTGTTCCTTCCGTCTCCATGTGTGCGTGTGTGCACGTATGCTATATCTTGCTGCTCTGCTCCATCCAGCTGTCACCTCTCCAGAGAGGCCTTCCCTAGCCATCCTCACCCCCTACTCTTTCACTTCGCCTTATCTGCCCTCCCTTGTGCTTCTGGCTCTCTGACTACACACGAGATGTTTGTTCTCTGCTCTGTGTCTCTGCCCACTGCTGGAGCACCACGACTTTGACTTGTTCTCTACGACCTAGTGCTCAGAAAAGCGCTCCTGCACATAGAAGGCCCTGAATAAATATTTGTTGAATGAATGAACAGCCACCACCGTGGCCATAAGAGGTTAGGCTGTTCACAGCTCCCAGGGCTGCTGGCGGCCCCGTGGAGGAGCGCAGTGGACAGTTTC";
  long count = first.length() * second.length();
  std::mutex enqueue, dequeue, stmt; //stmt is just for testing
  std::cout << "First String Length: " << first.length() << std::endl;
  std::cout << "Second String Length: " << second.length() << std::endl;
  //Setting up the matrix
  std::vector<std::vector<int> > grid((second.length()+1), std::vector<int>((first.length()+1), INT_MIN));
  std::vector<std::thread> threads(THREADS);
  fillInMatrix(std::ref(grid), first, second);
  //create the pool of squares from the matrix
  poolItem tmp;
  tmp.addItem(1,1);
  std::vector<poolItem> pool;
  pool.push_back(tmp);
  std::atomic<int> size(pool.size());

  //Setting up the threads
  for (int i = 0; i < THREADS; ++i)
    {
      threads[i] = std::thread(thread_function, std::ref(grid), first, second, std::ref(count), std::ref(enqueue), &pool, std::ref(stmt), std::ref(dequeue));
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
