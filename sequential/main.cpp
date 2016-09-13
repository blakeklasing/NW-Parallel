/*
    Blake Klesing
    Stacy Gramajo
    Sequential Needleman�Wunsch algorithm
    September 12, 2016
*/

#include <iostream>
#include <string.h>
#include <algorithm>
#include <vector>

#define MATCH 2
#define MISMATCH -1
#define GAP -1
#define DEFAULT -1

void printArray(std::vector<std::vector<int> > matrix, std::string text1, std::string text2)
{
    //Print out the first string
    std::cout << "\t\t";
    for(int i =0; i < text1.length(); ++i)
    {
            std::cout << text1.at(i) << "\t";
    }
    std::cout << std::endl;

    //print out the matrix and second string
    for(int i = 0; i < matrix.size(); ++i)
    {
        if(i!=0)
        {
            std::cout << text2.at(i-1) << "\t";
        }
        else
        {
            std::cout << "\t";
        }
        for(int j = 0; j < matrix[0].size(); ++j)
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
    for(int i = 0; i < strlength2; ++i)
    {
        matrix[i][0] = DEFAULT * i;
    }

    //Fill in the first column
    for(int j = 0; j < strlength1; ++j)
    {
        matrix[0][j] =  DEFAULT * j;
    }

    //The first row/column of matrix does not count
    //therefore, for loops start at 1
    for(int i = 1; i < strlength2; ++i)
    {
        for(int j = 1; j < strlength1; ++j)
        {
            bool match = (second.at(i-1) == first.at(j-1))? true: false;
            int diag = matrix[i-1][j-1] + (match? MATCH: MISMATCH);
            int up = matrix[i-1][j] + GAP;
            int left = matrix[i][j-1] + GAP;
            matrix[i][j] = maximum(diag, up, left);
        }
    }
    //print array
    printArray(matrix, first, second);
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