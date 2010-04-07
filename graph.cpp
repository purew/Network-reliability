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
#include <list>
#include <cstdlib>

#include "misc.h"
#include "graph.h"
#include "MersenneTwister.h"
#include "ants.h"

////////////////////////////////////////////////////////////
//
//      The edge class
//
////////////////////////////////////////////////////////////


edge::edge( int n1, int n2, float _reliability, float _cost )
{
	if (n1<n2)
	{
		n[0] = n1;
		n[1] = n2;
	}
	else
	{
		n[0] = n2;
		n[1] = n1;
	}

    reliability = _reliability;
    cost = _cost;
    working = true;
    acoTau = 1;
}

void edge::reset()
{
	acoTau=1;
	cost=1;
	if ( working >= 0 )
		working = 1;

}

////////////////////////////////////////////////////////////
//
//      The GRAPH class
//
////////////////////////////////////////////////////////////


int Graph::biggestNodeId = 0;

void Graph::setEdgeReliability( double newReliability )
{
	std::vector<edge*>::iterator it;
		for ( it = edges.begin(); it < edges.end() ; ++it )
			(*it)->setReliability( newReliability );
}

int Graph::addEdge( edge *e )
{
	// Dont forget to initialize connectingEdges if this is a new graph
	// Use the static biggestNodeId as size (loadEdgeData has to be called first).
	if ( connectingEdges == 0 )
		connectingEdges = new std::vector<edge*>[biggestNodeId+1];

	// Check so that e does not already exist in the list
	std::vector<edge*>::iterator it;
	for ( it = edges.begin(); it != edges.end(); ++it)
		if ( *it == e )
			return 1;

	edges.push_back( e );
	const int *n = e->getNodes();
	int n0 = n[0], n1 = n[1];
	connectingEdges[n[0]].push_back(e);
	connectingEdges[n[1]].push_back(e);
	return 0;
}

void Graph::disableXEdges( unsigned int F )
{
	// Prevent infty-loop
	if ( F > edges.size() )
		F = edges.size();

	// First reset the edges
	std::vector<edge*>::iterator it;
	for ( it = edges.begin(); it < edges.end() ; ++it )
			(*it)->reset();


	//std::cout << "F="<<F << std::endl;
	while ( F > 0 )
	{
		int r = randomNbrGenerator.randExc( edges.size());
		if ( edges[r]->isWorking() )
		{
			edges[r]->disable();
			--F;
		}

	}
}

void Graph::hardResetEdges()
{
	std::vector<edge*>::iterator it;
	for ( it = edges.begin(); it < edges.end() ; ++it )
		(*it)->hardReset();

}


float Graph::estReliabilityMC( int t, bool rawFormat)
{
	// TODO, implement threading of this part? If so, make copies of connectedEdges (REAL COPIES, not just the pointers)


	int workingAllTerminalNetworks=0;
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
				(*it)->setWorking( 0 );
			}

		}

		// Keep an array for all visited nodes.
		bool nodeVisited[biggestNodeId+1];
		for ( int i=0; i<=biggestNodeId; ++i )
		{
			nodeVisited[i] = false;
		}

		// Basic failsafe
		/*if ( n1>biggestNodeId || n1<0 || n2>network->getBiggestNodeId() || n2<0 )
		{
			std::cout << "In estReliabilityMC: Illegal node-id.\n\tAborting\n";
			return ILLEGAL_NODE_ID;
		}*/

		// Is this a working two-terminal instance of the problem?
		int result = unfoldGraph( 0, connectingEdges,nodeVisited );

		// Is it a working all-terminal instance? (Are all nodes visited?)
		int allterminal=1;
		for ( int i=0; i<=biggestNodeId; ++i )
			if ( nodeVisited[i]==false )
				allterminal=0;
		if (allterminal)
			workingAllTerminalNetworks += 1;
	}

	if ( rawFormat )
	{
		//std::cout << (float)(workingAllTerminalNetworks)/t << " ";

	}
	else
	{
		std::cout << "All-terminal reliability = " << (float)(workingAllTerminalNetworks)/t  << ", calculated from "<< t <<" simulations\n";
	}

	return (float)workingAllTerminalNetworks/t;
}

bool Graph::unfoldGraph( int nc,  std::vector<edge*> *connectingEdges, bool *visitedNodes )
{
	//std::cout << "Iterating over edges connected to " << nc << std::endl;
	visitedNodes[nc] = true;

	std::vector<edge*>::iterator it;
	for ( it = connectingEdges[nc].begin(); it < connectingEdges[nc].end() ; ++it )
	{
		//std::cout << "  Checking "<<(*it)->n[0]<<(*it)->n[1]<<" Status="<<(*it)->isWorking()<<std::endl;
		if ( (*it)->isWorking() )
		{
			int newNode = (*it)->getConnectingNode(nc);

			// Are these nodes visited earlier?
			if ( visitedNodes[newNode] == false)
			{
				// Expand this new node
				bool result = unfoldGraph( newNode, connectingEdges, visitedNodes );
			}
		}
	}

	// If we reach this far, then we've expanded the neighbors without finding the destination
	return 0;
}


void doPercolationCalculation(Graph* network)
{

	// In percolation mode, we want to calculate R(x,p) where N*x=F edges are removed
	// immediately. p is the normal reliability per edge.
	float stepSizeP = 0.05;
	int iterations = 100; // Average over this many iterations
	int MCiterations = 1e3;	// Do this many monte carlo iterations in the reliability estimation

	std::cout << "Averaging over "<<iterations<<" instances\n";
	std::cout << "Doing "<<MCiterations<<" Monte Carlo steps per instance\n";
	std::cout << "Percentage done: " << std::flush;

	std::ofstream datafile( "data/percolation.plot", std::ios::out|std::ios::trunc );

	// Disable f number of edges
	for ( int f=0; f<network->getEdges()->size(); ++f )
	{
		for ( float p=0; p<1.0; p+=stepSizeP )
		{
			float reliability = 0;
			for ( int i=0;i<iterations; ++i )
			{
				// Disable N*x edges that will not take part of the simulation
				network->disableXEdges(f);

				network->setEdgeReliability( p );
				float newRel = network->estReliabilityMC( MCiterations, 1 );
				if ( newRel < 0 )
					std::cout << "Something went wrong in the reliability estimation\n";
				reliability += newRel;
				// Restore the disabled edges
				network->hardResetEdges();
			}

			// Get the mean
			reliability /= iterations;
			datafile << reliability << " ";
		}
		datafile << std::endl;

		// Progressbar
		std::cout << (int)((float)f/network->getEdges()->size()*100)<<" "<<std::flush;

	}
	std::cout << std::endl;
	datafile.close();
}

int acoFindOptimal( Graph *nw, int Nmax, int Cmax, int nbrAnts )
{
	// Some parameters for the ACO algorithm
	float Q = 1; 		// Determines the deltaTau
	float rho = 0.8; 	// How fast old trails evaporate
	int MCiterations = 1000;	// How many iterations performed in Monte carlo
								// TODO: Should probably depend on number of links


	// Initialize the pheromones to tau_0, by resetting the edges
	// Reset all edges to working state
	std::vector<edge*>::iterator it;
	for ( it = nw->getEdges()->begin(); it < nw->getEdges()->end() ; ++it )
		(*it)->reset();

	int allNodes = nw->getBiggestNodeId();

	for ( int N=0; N<Nmax; ++N )
	{

		// Calculate the denominator (sum of pheromones) in the expression for the probabilities
		float sumTaus[nw->getBiggestNodeId()];
		for ( int n=0; n<=nw->getBiggestNodeId(); ++n )
		{
			// Loop over all links connecting node n, and sum up the pheromones
			sumTaus[n] = 0;
			std::vector<edge*>::iterator it;

			for ( it = nw->getConnectingEdges(n)->begin(); it < nw->getConnectingEdges(n)->end() ; ++it )
				sumTaus[n] += (*it)->acoGetTau();

		}

		// Calculate the probabilites to choose each link
		for ( int n=0; n<=nw->getBiggestNodeId(); ++n )
		{
			std::vector<edge*>::iterator it;
			for (it = nw->getConnectingEdges(n)->begin(); it < nw->getConnectingEdges(n)->end(); ++it )
			{
				float p = (*it)->acoGetTau()/sumTaus[n];
				(*it)->acoSetPFromNode( n, p );

			}
		}


		// Generate K=nbrAnts solutions
		std::list<Ant> ants(nbrAnts);

		std::list<Ant>::iterator antIt;
		for ( antIt=ants.begin(); antIt != ants.end(); ++antIt )
		{
			//std::cout << "Placing a new ant...\n";
			// Start in a random node
			int currNode = randomNbrGenerator.randExc(nw->getBiggestNodeId());
			edge *nextEdgeForAnt=0;

			int nbrNodesVisited = 0;
			char visitedNodes[nw->getBiggestNodeId()+1];
			for (int i=0; i<=nw->getBiggestNodeId(); ++i)
				visitedNodes[i] =0;



			// Loop until all nodes have been visited.
			// This MUST be effective, the ant cannot keep walking
			// without reaching the last nodes...
			int visitedNodesInARow = 0;
			while ( nbrNodesVisited < allNodes )
			{
				// Is this node a new node?
				if ( visitedNodes[currNode]==false )
				{
					visitedNodes[currNode] = true;
					++nbrNodesVisited;
					visitedNodesInARow = 0;
					// Are we breaking the cost restraint with this new link?
					if ( (*antIt).getCost() > Cmax )
						break;
				}
				else
				{
					visitedNodesInARow += 1;
				}


				// If the ant has been walking for a long time without finding new nodes,
				// pick it up and place it on a not yet visited node.
				if ( visitedNodesInARow > Ant::maxWalkWithoutNewNode )
				{
					// find a not yet visited node:
					int n;
					for ( n=0; n <= nw->getBiggestNodeId(); ++n)
						if (visitedNodes[n]==false)
							break;
					currNode = n;
					continue;
				}


				// Continue to the next node

				// Pick a random number and do roulette wheel selection
				float rnd = randomNbrGenerator();
				float sumP = 0;

				std::vector<edge*>::iterator it;
				for (it = nw->getConnectingEdges(currNode)->begin();
					it < nw->getConnectingEdges(currNode)->end();
					++it )
				{
					sumP += (*it)->acoGetPFromNode( currNode );
					if ( rnd < sumP )
						break;
				}

				// *nextEdgeForAnt now points to our chosen edge.
				nextEdgeForAnt = *it;
				const int *n = nextEdgeForAnt->getNodes();
				int n0 = n[0], n1 = n[1];
				(*antIt).addEdge(nextEdgeForAnt);
				currNode = nextEdgeForAnt->getConnectingNode(currNode);

				//std::cout << "\tAnt goes to node "<<currNode<<std::endl;
				//std::cin.get();
			}
		}

		// Begin the global updating
		float bestCost = 0;

		// Evaluate each ant
		for ( antIt=ants.begin(); antIt != ants.end();  )
		{

			float cost = (*antIt).getCost() ;
			//std::cout << "Ant has cost "<<cost<<std::endl;
			// Does the ant adhere to the cost restraints?
			if ( (*antIt).getCost() > Cmax )
			{
				// remove this ant and move iterator forward
				antIt = ants.erase(antIt);
				std::cout << "Removed ant for breaking cost restraint\n";
				continue;

			}
			else if ( (*antIt).getCost() > bestCost )
			{
				bestCost = (*antIt).getCost();
				(*antIt).estReliabilityMC( MCiterations );
			}
			// And increase iterator
			++antIt;
		}

		// The remaining ants are all valid solutions

		// Perform the global updating rule

		// Loop over each link in each ant and update tau
		for (antIt = ants.begin(); antIt != ants.end(); ++antIt)
		{
			std::vector<edge*>::iterator edgeIt;
			std::vector<edge*> *antEdges = (*antIt).getEdges();
			for (edgeIt = antEdges->begin(); edgeIt != antEdges->end();++edgeIt)
			{
				// deltaTau is a matrix containing the deltaTau's for this global update
				// Go through all the links and add to deltaTau accordingly
				int n1 = (*edgeIt)->getConnectingNode();
				int n2 = (*edgeIt)->getConnectingNode(n1);

				float C = (*edgeIt)->getCost() / bestCost;
				float penalty = 0.9*(*antIt).getLatestReliability()+0.1;
				float deltaTau = Q*C*penalty;

				// The next step is to update the network's pheromone levels with this new deltaTau'
				float oldTau = (*edgeIt)->acoGetTau();
				float newTau = deltaTau + rho * oldTau;

				(*edgeIt)->acoSetTau( newTau );
			}
		}

		std::cout << "Starting new iteration\n";
	}

	return NO_ERROR;
}

enum filetype { TYPE_EDGES };

int Graph::loadEdgeData( const char* filename, bool quiet )
{
	// CLEANUP: Empty the old vectors first
	cleanup();

    std::ifstream file( filename );
    if ( file.is_open() )
    {
        // Opening file went oke

        std::string line;
        getline( file, line );
        double reliabilityPerNode;

        // Determine type of the file
        if ( line.find("type") == std::string::npos )
		{
			std::cout << "Incorrect file, could not find a type-specifier\nExample of first line: \"type: edges\"\n";
			return FILE_OPEN_ERROR;
		}
		int filetype;
		if ( line.find("edges") != std::string::npos )
        {
        	filetype = TYPE_EDGES;

			// Find out the reliability of each node
			getline( file, line );
			if ( line.find("prob:") == std::string::npos)
			{
				std::cout << "No prob-field found in file, aborting\nline: " << line;
				return FILE_OPEN_ERROR;
			}
			int pos = line.find(" ");
			line.erase( 0, pos+1 );
			reliabilityPerNode = atof( line.c_str() );

			// Start parsing the data
			while ( file.eof() == 0 )
			{
				getline( file, line );
				if ( line.length() >= 3 && line[0] != '#' )
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
					edge* e = new edge( n1, n2, reliabilityPerNode );
					edges.push_back( e );

					if (n1>2000 || n2>2000)
						std::cout << "Large n...\n";

					// For later optimization (let each node know what edges are connecting)
					// we want to know the largest node id.
					if ( n1>biggestNodeId )
						biggestNodeId = n1;
					if ( n2>biggestNodeId )
						biggestNodeId = n2;
				}
			}


        }
        else
        {
        	std::cout << "Could not determine type of file.\n Try looking in the example files on how to do it\n";
        	return FILE_OPEN_ERROR;
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
			//std::cout << (*it)->n[0] << " " << (*it)->n[1] << " " << biggestNodeId <<  std::endl;
			const int *n = (*it)->getNodes();
			connectingEdges[ n[0] ].push_back( *it ); // Add a copy of *it to connectingEdges
			connectingEdges[ n[1] ].push_back( *it );
		}

		/*for (int i=0; i<=biggestNodeId; ++i)
		{
			std::cout << "node "<<i<<" has the following edges\n";
			for (it = connectingEdges[i].begin(); it < connectingEdges[i].end() ; ++it )
			{
				std::cout << (*it)->n[0] << " " << (*it)->n[1] << std::endl;
			}
		}*/

		if ( !quiet )
		{
			std::cout << "   Loaded " << edges.size() << " edges\n";
		}
        return NO_ERROR;
    }
    else
        return FILE_OPEN_ERROR;
}

void Graph::cleanup()
{
	if ( connectingEdges != 0 )
	{
		/*for (int i=0;i<=biggestNodeId; ++i )
		{
			if ( connectingEdges[i].empty() == false )
			connectingEdges[i].clear();
		}*/
		delete [] connectingEdges;
		connectingEdges = 0;
	}


	// DO NOT DELETE EDGES HERE, do it in finalCleanup instaed
	/*for ( it = edges.begin(); it < edges.end() ; ++it )
	{
		delete *it;
	}
	edges.clear();
	*/
}

void Graph::finalCleanup()
{
	std::vector<edge*>::iterator it;
	for ( it = edges.begin(); it < edges.end() ; ++it )
	{
		delete *it;
	}
	edges.clear();
}


Graph::Graph()
{
	connectingEdges = 0;

}

Graph::~Graph()
{
	cleanup();
}



