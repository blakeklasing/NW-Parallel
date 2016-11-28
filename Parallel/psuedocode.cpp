-------- Used in all of the algorithms 
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

------------ defined at the top --- idk if you want to mention this
#define MATCH 5
#define MISMATCH -1
#define GAP -2

------ First Algorithm: Lock-based vector array

void findvalue(string first, string second, vector<vector<int> > & grid, int i,
               int j, vector<poolItem> * pool, mutex & dequeue)
{ 
    if(grid[i][j] != INT_MIN) //already calculated
      return;

    if(grid[i-1][j] != INT_MIN && grid[i-1][j-1] != INT_MIN && grid[i][j-1] != INT_MIN)
    {
      //First, calculate the three scores: diag, up, left
      grid[i][j] = maximum(diag, up, left);

      //add two other items from matrix
      if(grid.size() > (i+1) && grid[i+1][j] == INT_MIN)
      { 
        item1.addItem(i+1, j);
        lock();
        pool->push_back(item1);
        unlock();
      }
      if(grid[0].size() > (j+1) && grid[i][j+1] == INT_MIN)
      {
          item2.addItem(i, j+1);
          lock();
          pool->push_back(item2);
          unlock();
      }
    }
    else
    { //add the item back to the pool
      item1.addItem(i, j);
      pool->push_back(item1);
    }
}

--------------------- Second Method

void findvalue(string first, string second, vector<vector<int> > & grid, int i,
  int j, Node *&head, Node *&tail, mutex & enqueue)
  { //normal calculation that can be seen in the NW sequential program
    if(grid[i][j] != INT_MIN)//already been calculated or its being calculated
      return;

    if(grid[i-1][j] != INT_MIN && grid[i-1][j-1] != INT_MIN && grid[i][j-1] != INT_MIN)
    { //calculate the scores: diag, up, and left
      //Take the max score
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

-------------------- Third implementation
struct Node
{
  int row,column;
  std::atomic<Node*> *next;
};

void findvalue(string first, string second, vector<vector<int> > & grid, int i,
  int j, Node *head, Node *tail)
  { 
    if(grid[i][j] != INT_MIN)//already been calculated or its being calculated
      return;

    if(grid[i-1][j] != INT_MIN && grid[i-1][j-1] != INT_MIN && grid[i][j-1] != INT_MIN)
    {
      //Calculate the scores: diag, up, and left
      grid[i][j] = maximum(diag, up, left);//Find Max of the scores

      //add two other items from matrix
      if(grid.size() > (i+1) && grid[i+1][j] == INT_MIN)
        enq(i+1, j, tail, head);

      if(grid[0].size() > (j+1) && grid[i][j+1] == INT_MIN)
        enq(i, j+1, tail, head);
    }
    else
    { //add the item back to the pool
      enq(i, j, tail, head);
    }
  }
