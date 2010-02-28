/*
	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
*/



#include "graph.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "graph.h"

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
    working = true;
}


////////////////////////////////////////////////////////////
//
//      The GRAPH class
//
////////////////////////////////////////////////////////////


float graph::estReliabilityMC( int n1, int n2, int t )
{
	// TODO, implement threading of this part? If so, make copies of connectedEdges (REAL COPIES, not just the pointers)

	int workingNetworks=0;
	for ( int i=0;i<t; ++i )
	{
		// Reset all edges to working state
		std::vector<edge*>::iterator it;
		for ( it = edges.begin(); it < edges.end() ; ++it )
				(*it)->reset();

		// Make some edges fail, with i.i.d. bernoulli-RV's.
		for ( it = edges.begin(); it < edges.end() ; ++it )
		{
			if ( randomNbrGenerator() > (*it)->getReliability() )
			{
				// This link failed!
				(*it)->setWorking( false );
			}

		}

		// Keep an array for all visited nodes.
		bool nodeVisited[biggestNodeId];
		for ( int i=0; i<biggestNodeId; ++i )
		{
			nodeVisited[i] = false;
		}
		nodeVisited[n1] = true;

		// for each of the connecting and working edges in connectingEdges[n1]
		bool working = unfoldGraph( n1, n2, connectingEdges,nodeVisited );
		if (working)
			++workingNetworks;
	}
	std::cout << "Reliability = " << (float)(workingNetworks)/t << ", calculated from "<< t <<" simulations\n";
}

bool graph::unfoldGraph( int nc, int nf, std::vector<edge*> *connectingEdges, bool *visitedNodes )
{
	std::vector<edge*>::iterator it;
	for ( it = connectingEdges[nf].begin(); it < connectingEdges[nf].end() ; ++it )
	{
		//std::cout << "Iterating over edges connected to " << nc << std::endl;
		edge* e = *it;
		if ( (*it)->isWorking() )
		{
			int n1=(*it)->n[0];
			int n2=(*it)->n[1];

			// Is this edge connected to the final dest?
			if ( n1 == nf || n2 == nf )
				return true;

			// Are these nodes visited earlier?
			if ( visitedNodes[n1] == false )
			{
				// Will this edge lead to the destination? (recursion)
				bool foundDestination = unfoldGraph( n1, nf, connectingEdges, visitedNodes );
				if (foundDestination)
					return true;

				visitedNodes[n1] = true;
			}

			// Are these nodes visited earlier?
			if ( visitedNodes[n2] == false )
			{
				// Will this edge lead to the destination? (recursion)
				bool foundDestination = unfoldGraph( n2, nf, connectingEdges, visitedNodes );
				if (foundDestination)
					return true;

				visitedNodes[n2] = true;
			}
		}
	}

	// If we reach this far, then we've expanded the neighbors without finding the destination
	return false;
}

void graph::printEdges()
{
	// Loop through edges and print each

	// for
	//	std::cout << n[0] << " " << n[1] << std::endl;

}

int graph::loadEdgeData( char* filename )
{


    std::ifstream file( filename );
    if ( file.is_open() )
    {
        // Opening file went oke

		biggestNodeId=0;
        std::string line;

        while ( file.eof() == 0 )
        {
            getline( file, line );
            if ( line.length() >= 3 )
            {
                // Length seems correct, try decoding the edge
                // Format of line is "XX YY"

                // atoi will read XX and stop at whitespace
                int n1 = atoi( line.c_str() );

                // For YY we need to find the whitespace first
                int pos = line.find(" ");
                line.erase( 0, pos+1 );
                int n2 = atoi( line.c_str() );

                // Add the edge to our vector
                edge* e = new edge( n1, n2 );
                edges.push_back( e );

                // For later optimization (let each node know what edges are connecting)
				// we want to know the largest node id.
				if ( n1>biggestNodeId )
					biggestNodeId = n1;
				if ( n2>biggestNodeId )
					biggestNodeId = n2;
            }
        }
        file.close();


        // Given a node, we want to quickly find what edges are connecting to this node
        // Thus we keep an array of vectors, where each element in the array corresponds
        // to a node and holds a linked vector with all the connecting edges
        connectingEdges = new std::vector<edge*>[biggestNodeId+1];

        // Now, go through edges and put each edge in the right element in connectingEdges
		std::vector<edge*>::iterator it;
		for ( it = edges.begin(); it < edges.end() ; ++it )
		{
			connectingEdges[ (*it)->n[0] ].push_back( *it ); // Add a copy of *it to connectingEdges
			connectingEdges[ (*it)->n[1] ].push_back( *it );

		}

        std::cout << "   Loaded " << edges.size() << " edges\n";
        return NO_ERROR;
    }
    else
        return FILE_OPEN_ERROR;
}


graph::~graph()
{

	std::vector<edge*>::iterator it;
	for ( it = edges.begin(); it < edges.end() ; ++it )
	{
			delete *it;
			*it = 0;
	}
	// Why is the below delete causing seg-faults?
	//delete[] connectingEdges;

}





////////////////////////////////////////////////////////////
//
//      The NODE class
//
////////////////////////////////////////////////////////////



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
