#include <xran.h>
#include <math.h>
#include <partition2.h>
#include <vector.h>
#include <xstring.h>
#include <xstring.cpp>
#include <fstream>
#include <iomanip>
#include <iostream>

const uint8_t NORTH = 0x01;
const uint8_t EAST  = 0x02;
const uint8_t SOUTH = 0x04;
const uint8_t WEST  = 0x08;
//random number generator
static fsu::Random_unsigned_int ran;

void Connect(size_t, size_t, size_t, size_t, 
            fsu::Vector<uint8_t>&, fsu::Partition<size_t>&);
void WriteMaze(std::ofstream&, size_t, size_t, size_t, size_t, fsu::Vector<uint8_t>);

int main (int argc, char* argv[])
{ 
    if(argc < 4)
    {
        std::cout << "** command line arguments required:\n"
        << "   1: number of rows\n"
        << "   2: number of cols\n"
        << "   3: file name for maze\n";
        return 0;
    }       
    //save the variables sent through the arguments
  size_t numrows = atoi(argv[1]);
  size_t numcols = atoi(argv[2]);
  //save the name and add the . to the name of the file
  fsu::String stringName = argv[3];
  stringName = stringName + ".";
  //print the variables
  size_t numcells = numrows * numcols;
  size_t start = ceil(numrows/2) * numcols;
  size_t goal = start + (numcols - 1);
  std::cout << "Welcome to ranmaze.\n";
  std::cout << " numrows  = " << numrows;
  std::cout << "\n numcols  = " << numcols;
  std::cout << "\n numcells = " << numcells;
  std::cout << "\n start    = " << start;
  std::cout << "\n goal     = " << goal;
  std::cout << "\n trace    = 0\n";
  //initialze the vector which will save the maze and partiton which will save
  //the path solution on the maze
  fsu::Vector<uint8_t> maze (numcells, 15);
  fsu::Partition<size_t> p (numcells);

  //run connect to get path from start to goal
  std::cout << " components after 0 passes:   " << p.Components() << '\n';
  Connect(start, goal, numrows, numcols, maze, p);
  size_t components = p.Components();
  std::cout << " components after 1 pass:     " << components << '\n';

  size_t digits = 1 + (size_t)log10(components);
  if(components > 1)
  {
    std::ofstream out1;
    //generate the correct name of the file with the solution of the maze
    fsu::String temp;
    for(size_t i = 0; i < digits; ++i)
    {
      char digit = '0' + (char)(components % 10);
      temp = temp + digit;
      components /= 10;
    }
    //swap the numbers to get the correct name
    for(size_t i = 0; i < (temp.Length()/2); ++i)
      fsu::Swap(temp[i], temp[temp.Length() - i - 1]);
    stringName = stringName + temp;
    //open the file to save the first solution
    out1.open(stringName.Cstr());
    if(out1.fail())
    {
       std::cout << " ** cannot open file " << stringName << '\n'
                 << " ** try again\n";
    return 0;
    }
    //write the "wall of code" to the first file
    WriteMaze(out1, start, goal, numrows, numcols, maze);
    out1.close();
    out1.clear();
    std::cout << " 1-pass maze written to file " << stringName << '\n';
    //run connect again to get the solution with all cells connected
    for(size_t cell = 1; cell < numcells; ++cell)
      Connect(0, cell, numrows, numcols, maze, p);
  }
  //save the name of the second file as file.1
  fsu::String filec1 = argv[3];
  filec1 = filec1 + ".1";
  std::ofstream out2;
  std::cout << " components after all passes: " << p.Components() << '\n';
  out2.open(filec1.Cstr());
  if(out2.fail())
  {
    std::cout << " ** cannot open file " << filec1 << '\n'
              << " ** try again\n";
    return 0;
  }
  //write to the new file the "wall of code"
  WriteMaze(out2, start, goal, numrows, numcols, maze);
  out2.close();
  out2.clear();
  std::cout << " n-pass maze written to file " << filec1 << '\n';
}
void WriteMaze(std::ofstream& out, size_t start, size_t goal, 
              size_t numrows, size_t numcols, fsu::Vector<uint8_t> maze)
  {
    out << '\t' << numrows << '\t' << numcols << '\n';
    size_t k = 0;
    for(size_t i = 0; i < numrows; i++)
    {
      for(size_t j = 0; j < numcols; j++)
      {
        out << '\t' << unsigned(maze[k]);
        k++;
      }
      out << '\n';
    }
    out << '\t' << start << '\t' << goal << '\n';
  }

void Connect(size_t start, size_t goal, size_t numrows, size_t numcols, 
                fsu::Vector<uint8_t>& maze, fsu::Partition<size_t>&p)
  {
    size_t numcells = numrows * numcols;
    size_t nextCell;
    size_t zero = 0;
    while(!p.Find(start, goal))
      {
        //get a random number for a cell
        size_t cell = ran(0, (numcells - 1));
        //get a direction
        size_t dir = ran(0, 4); //NESW
        //make sure cell is not a boundary wall
        if((cell >= zero) && (cell < numcols) && (dir == 0)) //north wall
          continue;
        else if(((cell + 1) % numcols == 0) && (dir == 1)) //East wall
          continue;  
        else if((cell >= (cell - numcols)) && (dir == 2)) //South wall
          continue;
        else if(((cell % numcols) == 0) && (dir == 3)) //West wall
          continue;
        //get the adjacent cells for getting a route from cell a to b
        if(dir == 0)
          nextCell = (cell - numcols);
        else if(dir == 1)
          nextCell = (cell + 1);
        else if(dir == 2)
          nextCell = (cell + numcols);
        else
          nextCell = (cell - 1);
        if(!p.Find(cell, nextCell))
        {
          //remove wall from maze meaning change the number in the cell
          //have to change the walls from 2 cells
          //if the direction is north
          if(dir == 0)
          {
            maze[cell] &= ~NORTH;
            maze[nextCell] &= ~SOUTH;
          }//if the direction is east knowck down both walls
          else if(dir == 1)
          {
            maze[cell] &= ~EAST;
            maze[nextCell] &= ~WEST;
          }//if the direcction is south knock down both walls
          else if(dir == 2)
          {
            maze[cell] &= ~SOUTH;
            maze[nextCell] &= ~NORTH;
          }
          else//if the direction is west knock down both walls
          {
            maze[cell] &= ~WEST;
            maze[nextCell] &= ~EAST;
          }//add both the cells(current and adjacent) to the solution partition
           p.Union(cell, nextCell);
        }
      }
  }