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
#include <cmath>

#include "misc.h"
#include "graph.h"
#include "MersenneTwister.h"
#include "ants.h"

////////////////////////////////////////////////////////////
//
//      The edge class
//
////////////////////////////////////////////////////////////


Edge::Edge( int n1, int n2, float _reliability, float _cost )
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

	acoTau = new float[maxLevels];
    for ( int i=0; i<maxLevels; ++i )
    {
		acoTau[i] = 1;
    }
	//std::cout << "Calling edge constructor... " << this<< " "<<acoTau[0]<<" "<<acoTau[1]<<std::endl;

}
void Edge::setTau(int level, float tau )
{
	if (level>=maxLevels)
		throw "level is too big in setTau";
	acoTau[level] = tau;


}
void Edge::reset()
{

	cost=1;
	if ( working >= 0 )
		working = 1;
}
void Edge::hardReset()
{
	for ( int i=0; i<maxLevels; ++i )
	{
		acoTau[i] = 1;
	}
	 working=1;
	reset();
}
float Edge::getSumTau()
{
	float sum=0;
	for (int i=0; i<maxLevels; ++i)
		sum += acoTau[i];
	return sum;
}

Edge::~Edge()
{
	delete[] acoTau;
	acoTau =0;
}

////////////////////////////////////////////////////////////
//
//      The GRAPH class
//
////////////////////////////////////////////////////////////


int Graph::biggestNodeId = 0;

void Graph::setEdgeReliability( double newReliability )
{
	std::vector<Edge*>::iterator it;
		for ( it = edges.begin(); it < edges.end() ; ++it )
			(*it)->setReliability( newReliability );
}

int Graph::addEdge( Edge *e )
{


	// Dont forget to initialize connectingEdges if this is a new graph
	// Use the static biggestNodeId as size (loadEdgeData has to be called first).
	if ( connectingEdges == 0 )
		connectingEdges = new std::vector<Edge*>[biggestNodeId+1];

	// Check so that e does not already exist in the list
	std::vector<Edge*>::iterator it;
	for ( it = edges.begin(); it != edges.end(); ++it)
		if ( *it == e )
			return 1;

	edges.push_back( e );
	const int *n = e->getNodes();
	int n0 = n[0], n1 = n[1];
	connectingEdges[n[0]].push_back(e);
	connectingEdges[n[1]].push_back(e);

	// Network has changed, the estimated reliability does not apply anymore
	latestEstimatedReliability = -1;
	return 0;
}

void Graph::disableXEdges( unsigned int F )
{
	// Prevent infty-loop
	if ( F > edges.size() )
		F = edges.size();

	// First reset the edges
	std::vector<Edge*>::iterator it;
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
	std::vector<Edge*>::iterator it;
	for ( it = edges.begin(); it != edges.end() ; ++it )
		(*it)->hardReset();

}


float Graph::estReliabilityMC( int t, bool rawFormat)
{
	// TODO, implement threading of this part? If so, make copies of connectedEdges (REAL COPIES, not just the pointers)


	int workingAllTerminalNetworks=0;
	for ( int i=0;i<t; ++i )
	{
		// Reset all edges to working state
		std::vector<Edge*>::iterator it;
		for ( it = edges.begin(); it != edges.end() ; ++it )
		{
			(*it)->reset();
		}
		// Make some edges fail, with i.i.d. bernoulli-RV's.
		for ( it = edges.begin(); it != edges.end() ; ++it )
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

	latestEstimatedReliability = (float)workingAllTerminalNetworks/t;
	return latestEstimatedReliability;
}

bool Graph::unfoldGraph( int nc,  std::vector<Edge*> *connectingEdges, bool *visitedNodes )
{
	//std::cout << "Iterating over edges connected to " << nc << std::endl;
	visitedNodes[nc] = true;

	std::vector<Edge*>::iterator it;
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
	float Q = 4; 		// Determines the deltaTau
	float rho = 0.9; 	// How fast old trails evaporate
	int MCiterations = 10000;	// How many iterations performed in Monte carlo
								// TODO: Should probably depend on number of links
	float a = 2;		// Exponent to C/C*


	Ant *bestAnt=0;

	// Initialize the pheromones to tau_0, by resetting the edges
	// Reset all edges to working state
	std::vector<Edge*>::iterator it;
	for ( it = nw->getEdges()->begin(); it != nw->getEdges()->end() ; ++it )
		(*it)->hardReset();

	int allNodes = nw->getBiggestNodeId();
'


	// Generate K=nbrAnts solutions
	std::list<Ant> ants;
	for (int i=0; i < nbrAnts; ++i)
	{
		// Initialize the list by allocating new objects
		int size = nw->getEdges()->size();
		Ant *ant = new Ant(size);
		ants.push_back(*ant);
	}

	for ( int N=0; N<Nmax; ++N )
	{






		std::list<Ant>::iterator antIt;
		for ( antIt=ants.begin(); antIt != ants.end(); ++antIt )
		{
			std::vector<Edge*> &edges = *(nw->getEdges());
			int size = edges.size();
			for ( int i=0; i<size; ++i )
			{
				// Calculate the probability to choose this link
				float sumTaus = edges[i]->getSumTau();
				float p=		edges[i]->getTau(1) / sumTaus;

				// p is now the probability that this edge is working (level=1)
				// This assumes only on/off state of the link

				if ( p > randomNbrGenerator() )
				{
					(*antIt).addEdge( edges[i] );
					(*antIt).setLinkLevel( i, 1);

				}
				else
					(*antIt).setLinkLevel( i, 0);
			}
		}

		// Begin the global updating
		float bestCost = 0;
		float bestReliability = 0;

		// Evaluate each ant
		for ( antIt=ants.begin(); antIt != ants.end();  )
		{

			//std::cout << "size of antIt edges: " <<(*antIt).getEdges()->size();
			float cost = (*antIt).getCost() ;
			float reliability = (*antIt).estReliabilityMC( MCiterations, true );

			// Does the ant adhere to the cost restraints?
			if ( cost > Cmax )
			{
				// remove this ant and move iterator forward
				//antIt = ants.erase(antIt);
				//std::cout << "Removed ant for breaking cost restraint\n";
				//continue;
			}
			else if ( reliability > bestReliability )
			{
				bestCost = cost;
				bestReliability = reliability;

				bestAnt = &(*antIt);
			}

			//std::cout << "Rel: "<< reliability <<" cost: "<<cost<<std::endl;
			// And increase iterator
			++antIt;
		}


		if ( bestAnt!=0  && N+1==Nmax)
			bestAnt->printEdges();
		if ( N+1 == Nmax )
			bestReliability = bestAnt->estReliabilityMC(10*MCiterations, true);
		std::cout << "Best reliability "<<bestReliability<< " cost: "<<bestCost<<std::endl;


		// The remaining ants are all valid solutions

		// Perform the global updating rule

		// Loop over each link in each ant and update tau
		for (antIt = ants.begin(); antIt != ants.end(); ++antIt)
		{


			// Iterate over ALL edges and apply pheromones accordingly
			// This means, iterate over ant. which says if link i is working or not
			std::vector<Edge*> *allEdges = nw->getEdges();
			int maxLinks = allEdges->size();
			for ( int i=0; i<maxLinks; ++i )
			{
				for ( int level=0; level<Edge::maxLevels; ++level )
				{
					float deltaTau;
					if ( (*antIt).getLinkLevel(i) == level )
					{

						float C = (*allEdges)[i]->getCost() / bestCost;
						C		= pow( C, a );
						float reliability = (*antIt).getLatestReliability();
						float penalty = 5*reliability+0.1;
						deltaTau = Q*C*penalty;
					}
					else
					{
						deltaTau = 0;
					}
					// The next step is to update the network's pheromone levels with this new deltaTau'
					float oldTau = (*allEdges)[i]->getTau( level );
					float newTau = deltaTau + rho * oldTau;

					(*allEdges)[i]->setTau( level, newTau );


				}
				//std::cout << "New tau for "<< i<<": "<< (*allEdges)[i]->getTau( 1 )<<std::endl;
			}

		}

		//std::cout << "Starting new iteration\n";
	}


	return NO_ERROR;
}

enum filetype { TYPE_EDGES };

int Graph::loadEdgeData( const char* filename, bool quiet )
{
	// CLEANUP: Empty the old vectors first
	cleanup();
	latestEstimatedReliability = -1;

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
					Edge* e = new Edge( n1, n2, reliabilityPerNode );
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
		connectingEdges = new std::vector<Edge*>[biggestNodeId+1];

        // Now, go through edges and put each edge in the right element in connectingEdges
		std::vector<Edge*>::iterator it;
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


float Graph::getLatestReliability()
{

	if (latestEstimatedReliability < 0 )
		estReliabilityMC(1000, true);
	return latestEstimatedReliability;
}

void Graph::printEdges()
{
	std::cout << "Edge "<<this<<std::endl;
	std::vector<Edge*>::iterator edgeIt;
	for (edgeIt = edges.begin(); edgeIt != edges.end(); ++edgeIt)
	{
		int n0 = (*edgeIt)->getConnectingNode();
		int n1 = (*edgeIt)->getConnectingNode(n0);
		std::cout <<"  "<< n0 << " "<<n1<<" tau: "<<(*edgeIt)->getTau(1)/(*edgeIt)->getSumTau()<<std::endl;
	}
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
	std::vector<Edge*>::iterator it;
	for ( it = edges.begin(); it < edges.end() ; ++it )
	{
		delete *it;
	}
	edges.clear();
}


Graph::Graph()
{
	if (biggestNodeId != 0)
	{
		// The network is already initialized
		connectingEdges = new std::vector<Edge*>[biggestNodeId+1];
	}
	else
		connectingEdges = 0;

	latestEstimatedReliability = -1;

}

Graph::~Graph()
{
	cleanup();
}



