#include <graph.h>
#include <graph_util.h>
#include <bfsurvey.h>
#include <dfsurvey.h>
#include <survey_util.h>
#include <maze_util.h>
#include <fstream>
#include <xstring.h>
#include <xstring.cpp>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << " ** command line argument required:\n"
                << "    1: mazefile\n"
                << " ** try again\n\n";
        return EXIT_FAILURE;
    }
    char * maze = argv[1];
    fsu::ALDGraph<size_t> graph;
    size_t start, goal;
    //load the maze to computer symmertry and dfs
    if(!LoadMaze (maze, graph, start, goal))
    {
        return EXIT_FAILURE;
    }
    //check for symmetry
    if(!CheckSymmetry(graph, 0))
        std::cout << "Maze is not symmertic\n";
    typename fsu::ALDGraph<size_t>::Vertex s(start), g(goal);
    fsu::List<fsu::ALDGraph<size_t>::Vertex> solution;
    //find the path using DFS
    if(!Path_DFS(graph, s, g, solution))
        std::cout << " NO Solution\n";

    //set up the name of the new file
    char name[] = ".dfs";
    char * newName = new char(std::strlen(maze) + std::strlen(name) + 1);
    std::strcpy(newName, maze);
    std::strcat(newName, name);
    
    //read from(source)
    std::ifstream in1;
    in1.open(maze);
    if(in1.fail())
    {
        std::cout << "File does not exist\n";
        return EXIT_FAILURE;
    }
    //write to(target) from the beginning
    std::ofstream out1;
    out1.open(newName);
    out1 << in1.rdbuf();
    out1 << '\n';    
    out1.close();

    out1.open(newName, std::ios_base::app);
    //append the content of the soltion to the file with running the list
    //fromt beginning to the end and writing it to the file
    fsu::List<fsu::ALDGraph<size_t>::Vertex>::Iterator i = solution.Begin();
    out1 << " ";
    for(; i != solution.End(); ++i)
        out1 << *i << " ";
    out1.close();
    in1.close();

    std::cout << "Solution found has " << solution.Size() << " cells\n";
    std::cout << "Maze and solution written to file \"" << newName << "\"\n";

    return 0;
}