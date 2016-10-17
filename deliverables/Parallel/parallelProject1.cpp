/*
    Parallel Program
    Stacy Gramajo
    COP 6616
    Needleman-Wunch Algorithm
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <math.h>
#include <fstream>
#include <time.h>
#include <algorithm>

#define THREADS 2
#define MATCH 2
#define MISMATCH -1
#define GAP -2

class poolItem
{
  public:
    int row, column;
    void addItem(int, int);
};

void poolItem::addItem(int i, int j)
{
    row = i;
    column = j;
}

class matrix
{
  public:
    int value, loc_i, loc_j;
    void set_value(int);
    void set_ij(int, int);
    void set_all(int, int, int);
};

void matrix::set_all(int x, int y, int z)
{
    value = x;
    loc_i = y;
    loc_j = z;
}

void matrix::set_ij (int i, int j)
{
    loc_i = i;
    loc_j = j;
}

void matrix::set_value(int val)
{
    value = val;
}

int maximum(int one, int two, int three)
{
    int maxNum = std::max(one, two);
    return std::max(maxNum, three);
}

void findvalue(std::string first, std::string second, std::vector<std::vector<matrix> > & grid, int i, int j, std::vector<poolItem> & pool)
{
    bool match = (second.at(i-1) == first.at(j-1))? true: false;
    int diag = grid[i-1][j-1].value + (match? MATCH:MISMATCH);
    int up = grid[i-1][j].value + GAP;
    int left = grid[i][j-1].value + GAP;
    grid[i][j].set_value(maximum(diag, up, left));

    //add two new items into the array - bottom and to the right
    int tmp = 0;
    poolItem item1;
    poolItem item2;
    while(count != 2)
    { //first check the bottom box -- not out of bounds
        if(grid.size() > (i+1))
        { //add and count++
          item1.addItem(i+1, j);
          pool.push_back(item1);
          tmp++;
        }
        else
        {//don't add the bottom cell - only increase the tmp value
          tmp++;
        }
        //add the second item
        if(grid[0].size() > (j+1))
        {
            if(grid[i-1][j+1].value != null)
            {
              item2.addItem(i, j+1);
              pool.push_back(item2);
              tmp++;
            }
        }
        else
        {//don't add the left cell - only increase the tmp value;
          tmp++;
        }

    }//end of while stmt
}
void thread_function(std::vector<std::vector<matrix> > & grid, std::string first, std::string second, long & count,
                     std::mutex & lock, std::vector<poolItem> & pool)
{
    bool temp = false;
    poolItem item;
    while(count > 0)
    {
        lock.lock();
        if(!pool.empty())
        {
            item = pool.front();
            pool.erase(0); //erase first item from the vector
            temp = true;
            count--;
        }
        lock.unlock();

        if(temp)
        {
          findvalue(first, second, std::ref(grid), item.row, item.column, std::ref(pool));
          temp = false;
        }
        else
        { //wait a second to search the pool again
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void printArray(std::vector<std::vector<matrix> > grid, std::string first, std::string second)
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
            std::cout << grid[i][j].value << "\t";
        }
        std::cout << std::endl;
    }
}

void fillInMatrix(std::vector<std::vector<matrix> > & grid, std::string first, std::string second)
{
  /*An extra row and column was added for the gap score*/
    int strlength1 = first.length() + 1;
    int strlength2 = second.length() + 1;
    grid.resize(strlength2);
    for(int k = 0; k < strlength2; ++k)
    {
        grid[k].resize(strlength1);
    }

    //First fill in the top row
    for(int i = 0; i < strlength2; ++i)
    {
        grid[i][0].set_all(GAP * i, i, 0);
    }

    //Fill in the first column
    for(int j = 0; j < strlength1; ++j)
    {
        grid[0][j].set_all(GAP * j, 0, j);
    }
}

int main(int argc, char *argv[])
{
  std::string first = "CTTCA";
  std::string second = "CTACA";
  long count = first.length() * second.length();
  std::mutex lock;
  std::cout << "First String: " << first << std::endl;
  std::cout << "Second String: "<< second << std::endl;
  //Setting up the matrix
  std::vector<std::vector<matrix> > grid;
  std::vector<std::thread> threads(THREADS);
  fillInMatrix(std::ref(grid), first, second);
  //create the pool of squares from the matrix
  poolItem tmp;
  tmp.addItem(1,1);
  std::vector<poolItem> pool;
  pool.push_back(tmp);

  //Setting up the threads
  for (int i = 0; i < THREADS; ++i)
	{
  	//threads[i] = std::thread(thread_function, std::ref(grid), first, second, std::ref(count), std::ref(lock), ref(pool));
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

  std::cout << "Time Elapse: " << diffms << std::endl;
}