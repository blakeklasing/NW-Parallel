/*
    Blake Klesing
    Stacy Gramajo
    Sequential Needleman–Wunsch algorithm
    September 12, 2016
*/

#include <iostream>
#include <string.h>
#include <algorithm>
#include <vector>

#define MATCH 5
#define MISMATCH -1
#define GAP -2
#define DEFAULT -2

void printArray(std::vector<std::vector<int> > matrix)
{
    for(int i = 0; i < matrix[0].size(); ++i)
    {
        for(int j = 0; j < matrix.size(); ++j)
        {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

int maximum(int one, int two, int three)
{
    int maxNum = std::max(one, two);
    return std::max(maxNum, three);
}

/*
        This function still needs work
        --- compared it to matrixs online and there were some num hat were off
*/
void fillInMatrix(std::string first, std::string second)
{
    int strlength1 = first.length() + 1;
    int strlength2 = second.length() + 1;
    std::vector<std::vector<int> > matrix;
    matrix.resize(strlength2);
    for(int k = 0; k < strlength2; ++k)
    {
        matrix[k].resize(strlength1);
    }

    //First fill in the top row
    for(int i = 0; i < strlength1; ++i)
    {
        matrix[i][0] = DEFAULT * i;
    }

    //Fill in the first column
    for(int j = 0; j < strlength2; ++j)
    {
        matrix[0][j] =  DEFAULT * j;
    }
    //The first row/column of matrix does not count
    //therefore, for loops start at 1
    for(int i = 1; i < strlength1; ++i)
    {
        for(int j = 1; j < strlength2; ++j)
        {
            bool match = (first.at(i-1) == second.at(j-1))? true: false;
            int diag = matrix[i-1][j-1] + (match? MATCH: MISMATCH);
            int up = matrix[i-1][j] + GAP;
            int left = matrix[i][j-1] + GAP;
            matrix[i][j] = maximum(diag, up, left);
        }
    }
    //print array
    printArray(matrix);
}

int main(int argc, char *argv[])
{
    std::string first, second;
    std::cout << "Input first string: " << std::endl;
    std::cin >> first;
    std::cout << "Input second string: " <<std::endl;
    std::cin >> second;

    fillInMatrix(first, second);
}
