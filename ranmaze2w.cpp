#include <xran.h>
#include <math.h>
#include <partition2.h>
#include <vector.h>
#include <xstring.h>
#include <xstring.cpp>
#include <fstream>
#include <iomanip>

const uint8_t NORTH = 0x01;
const uint8_t EAST  = 0x02;
const uint8_t SOUTH = 0x04;
const uint8_t WEST  = 0x08;

static fsu::Random_unsigned_int ran;

void Connect(size_t, size_t, size_t, size_t, 
            fsu::Vector<uint8_t>&, fsu::Partition<size_t>&);
void WriteMaze(std::ofstream&, size_t, size_t, size_t, size_t, fsu::Vector<uint8_t>);

int main (int argc, char* argv[])
{   //if the arguments are not correct
    if(argc < 4)
    {
        std::cout << "** command line arguments required:\n"
        << "   1: number of rows\n"
        << "   2: number of cols\n"
        << "   3: file name for maze\n";
        return 0;
    }       
    //save arguments
  size_t numrows = atoi(argv[1]);
  size_t numcols = atoi(argv[2]);
  //save the name as file.1
  fsu::String name = argv[3];
  name = name + ".1";
  //ccalculate the variables and print them to screen
  size_t numcells = numrows * numcols;
  size_t start = ceil(numrows/2) * numcols;
  size_t goal = start + (numcols - 1);
  std::cout << "RanMaze:  numrows = " << numrows << ", numcols = " 
  << numcols << ", numcells = " << numcells << ", start = " << start
  << ", goal = " << goal << ", trace = 0\n";

  //initialize the maze and partition to solve and save the maze
  fsu::Vector<uint8_t> maze (numcells, 15);
  fsu::Partition<size_t> p (numcells);

  //run the connect to connect from start to goal 
  std::cout << " components after 0 passes:   " << p.Components() << '\n';
  Connect(start, goal, numrows, numcols, maze, p);
  int components = p.Components();
  std::cout << " components after 1 pass:     " << components << '\n';
  //open the file to write the output with the "wall of code"
  std::ofstream out1;
  if(components > 1)
  {
    out1.open(name.Cstr());
    if(out1.fail())
    {
       std::cout << " ** cannot open file " << name << '\n'
                 << " ** try again\n";
    return 0;
    }
    //write the "wall of code" in file.1
    WriteMaze(out1, start, goal, numrows, numcols, maze);
    out1.close();
    out1.clear();
    for(size_t cell = 1; cell < numcells; ++cell)
      Connect(0, cell, numrows, numcols, maze, p);
  }
  std::cout << " components after all passes: " << p.Components() << '\n';
  return 0;
}
//function to write the maze into the output file
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
          {//take down both walls
            maze[cell] &= ~NORTH;
            maze[nextCell] &= ~SOUTH;
          }//if the direction is east
          else if(dir == 1)
          {//take down both walls
            maze[cell] &= ~EAST;
            maze[nextCell] &= ~WEST;
          }
          else if(dir == 2) //if the direction is south
          {//take down both walls
            maze[cell] &= ~SOUTH;
            maze[nextCell] &= ~NORTH;
          }
          else //if the direction is west
          {//take down both walls
            maze[cell] &= ~WEST;
            maze[nextCell] &= ~EAST;
          }//add the 2 cells in the parition p
           p.Union(cell, nextCell);
        }
      }
  }