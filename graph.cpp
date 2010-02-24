
#include "graph.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cstdlib>

////////////////////////////////////////////////////////////
//
//      The edge class
//
////////////////////////////////////////////////////////////


edge::edge( int n1, int n2, float _reliability, float _cost )
{
    n[0] = n1;n[1] = n2;
    reliability = _reliability;
    cost = _cost;
}


////////////////////////////////////////////////////////////
//
//      The GRAPH class
//
////////////////////////////////////////////////////////////

int graph::loadEdgeData( char* filename )
{
    std::ifstream file( filename );
    if ( file.is_open() )
    {
        // Opening file went oke

        std::string line;
        while ( file.eof() == 0 )
        {
            getline( file, line );
            if ( line.length() >= 3 )
            {
                // Length seems correct, try creating the edge
                // Format of line is "XX YY"

                // atoi will read XX and stop at whitespace
                int n1 = atoi( line.c_str() );

                // For YY we need to find the whitespace first
                int pos = line.find(" ");
                line.erase( 0, pos+1 );
                int n2 = atoi( line.c_str() );

                // Add the edge to our list
                edge e( n1, n2 );
                edges.push_front( e );
            }

        }
        file.close();

        std::cout << "   Loaded " << edges.size() << " edges\n";
        return NO_ERROR;
    }
    else
        return FILE_OPEN_ERROR;
}







 int node::gid = 1;

int node::connect( node *p )
{
    if (neighbors<maxNeighbors)
    {
        // We don't want duplicates
        for (int i=0;i<neighbors;++i)
            if ( neighbArr[i] == p )
                return DUPLICATE;

        neighbArr[neighbors] = p;
        ++neighbors;

        // This worked, now lets try to do add this node to *p
        int pResult = p->connect(this);
        if (pResult == NO_ERROR )
            return NO_ERROR;
        else
        {
            // There was an error adding *this to *p, reverse the actions on *this
            --neighbors;
            neighbArr[neighbors] = 0;
            return pResult;
        }
    }
    else return MAX_NEIGHBORS;
}

node::node( int _maxNeighbors )
{
    neighbors = 0;
    maxNeighbors = _maxNeighbors;
    id = gid;
    ++gid;


    neighbArr = new node*[maxNeighbors];
    for (int i=0;i<maxNeighbors; ++i)
        neighbArr[i] = 0;
}
node::~node( )
{
    delete[] neighbArr;
}


 void node::printConn()
{
    std::cout << id << " connected to ";
    for (int i=0;i<neighbors;++i)
        std::cout << neighbArr[i]->getID() << ", ";
    std::cout << "\n";
}
