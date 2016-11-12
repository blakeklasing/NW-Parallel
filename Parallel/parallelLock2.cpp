/*
    Parallel Program
    Stacy Gramajo
    COP 6616
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
#define THREADS 2
//Scoring
#define MATCH 5
#define MISMATCH -1
#define GAP -2
//for updating pool Array
#define ROW 1
#define COLUMN 2
#define BOTH 3

class poolItem
{
  public:
    int row, column, update;
    void addItem(int, int, int);
};

void poolItem::addItem(int i, int j, int updateValue)
{
    row = i;
    column = j;
    update = updateValue;
}

int maximum(int one, int two, int three)
{
    int maxNum = std::max(one, two);
    return std::max(maxNum, three);
}

void findvalue(std::string first, std::string second, std::vector<std::vector<int> > & grid, poolItem item,
               std::vector<poolItem> & pool, std::mutex & stmt, std::mutex & lock, int & currentI, int & currentJ)
{ //normal calculation that can be seen in the NW sequential program
    int i = item.row;
    int j = item.column;
    bool match = (second.at(i-1) == first.at(j-1))? true: false;
    int diag = grid[i-1][j-1] + (match? MATCH:MISMATCH);
    int up = grid[i-1][j] + GAP;
    int left = grid[i][j-1] + GAP;
    grid[i][j] = maximum(diag, up, left);
    //debugging --------------------
    stmt.lock();
    std::cout << "Finished Calculating: "<< "[" << i << "," << j << "]" << std::endl;
    stmt.unlock();
    //add two new items into the array - bottom and to the right
    //There are protocols added because of various situations that can occur
    int tmp = 0;
    poolItem item1;
    poolItem item2;
    //add another cell from column
    if(item.update == BOTH || item.update == COLUMN)
    {
      if(grid[0].size() > (j+1))
      {
        //  std::cout << "Added: i = " << i+1 << "\tj = " <<j << std::endl;
          item2.addItem(i, j+1, COLUMN);
          pool.push_back(item2);
      } //else, row completed - on to the next row to complete
    }
    //add another cell from row
    if(item.update == BOTH || item.update == ROW)
    {
        //std::cout << "i = " << i+1 << "\tj = " <<j << std::endl;
        if(grid.size() > (i+1))
        { //add and count++
          //std::cout << "Added: i = " << i+1 << "\tj = " <<j << std::endl;
          item1.addItem(i+1, j, ROW);
          pool.push_back(item1);
        }
        else
        { //reached the end of the row - now, add a diagonal
          lock.lock();
          if((currentJ+1) >= grid[0].size())
          { //we're done
            lock.unlock();
            return;
          }
          currentJ++;
          currentI++;
          std::cout << "Added: i = " << currentI << "\tj = " <<currentJ<< std::endl;
          while(grid[currentI-1][grid[0].size()-1] == INT_MIN)
          {
            std::cout << "Waiting " << std::endl; 
              //wait until the last one in the row has completed
          }
          item1.addItem(currentI, currentJ, BOTH);
          pool.push_back(item1);
          lock.unlock();
        }
      }

}
void thread_function(std::vector<std::vector<int> > & grid, std::string first, std::string second, long & count,
                     std::mutex & enqueue, std::vector<poolItem> & pool, std::mutex & stmt, std::mutex & dequeue, int & currentI, int & currentJ)
{
    bool temp = false;
    poolItem item;
    while(count > 0)
    {
        //need to lock the pool because another thread might retrieve the item
        dequeue.lock();
        if(!pool.empty())
        {
            item = pool.front();
            pool.erase(pool.begin()); //erase first item from the vector
            temp = true;
            count--;
        }
        dequeue.unlock();

        if(temp)
        { //Calculate the matrix item
          findvalue(first, second, std::ref(grid), item, std::ref(pool), std::ref(stmt), std::ref(enqueue), std::ref(currentI), std::ref(currentJ));
          temp = false;
        }
    }
}

void printArray(std::vector<std::vector<int> > grid, std::string first, std::string second)
{
    //First, printout the first string
    std::cout << "\t\t";
    for(int i = 0; i < first.length(); ++i)
    {
        std::cout << first.at(i) << "\t";
    }
    std::cout << std::endl;

    //print out the matrix and second string
    for(int i = 0; i < grid.size(); ++i)
    {
        if(i!=0)
        {
            std::cout << second.at(i-1) << "\t";
        }
        else
        {
            std::cout << "\t";
        }
        for(int j = 0; j < grid[0].size(); ++j)
        {
            std::cout << grid[i][j] << "\t";
        }
        std::cout << std::endl;
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
  int currentI = 1;
  int currentJ = 1;
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
  tmp.addItem(1,1,BOTH);
  std::vector<poolItem> pool;
  pool.push_back(tmp);
  //Setting up the threads
  for (int i = 0; i < THREADS; ++i)
	{
  	threads[i] = std::thread(thread_function, std::ref(grid), first, second, std::ref(count), std::ref(enqueue),
                              std::ref(pool), std::ref(stmt), std::ref(dequeue), std::ref(currentI), std::ref(currentJ));
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
  // Check the time
  clock_t end_time = clock();
  float diffticks = end_time - begin_time;
  float diffms = (diffticks) / CLOCKS_PER_SEC;
//  printArray(grid, first, second);
  std::cout << "Time Elapse: " << diffms << std::endl;
  std::cout << "Alignment Score: " << grid[second.length()][first.length()] << std::endl;
}
