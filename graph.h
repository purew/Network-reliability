/** @file graph.h
 *
 * Classes for handling the structure of the network. They are assumed
 * to be static once created but this assumption is purely for faster
 * execution and simplicity of code.


 	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php


 */


#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>

#include "Mersenne-1.1/MersenneTwister.h"

enum errors {   NO_ERROR=0,
                DUPLICATE,
                MAX_NEIGHBORS,
                FILE_OPEN_ERROR,
                ILLEGAL_NODE_ID
};


/** A class representing the edge in our graph.
    The edge goes both ways. */
class edge
{
public:
	/** Constructor takes two nodes as argument, and optionally reliability and cost. */
    edge( int n1, int n2, float reliability = 0.8, float cost=1.0 );
    /** Returns an array with two elements containing the connected nodes. */
    const int* getNodes() { return n; };
	/** Reset the node to working condition. */
	void reset() {working = true;};
	bool isWorking() {return working;};
	void setWorking( bool status ) {working = status;};
	float getCost() {return cost;};
	double getReliability() {return reliability;};

    int n[2];
private:

	bool working;

    float cost;
    double reliability;
};

/** Class for keeping track of the graph.
    Performs loading, cleanup, etc. */
class graph
{
public:
	/** Load a nwk-file in the format:
	x1 x2
	x1 x3
	etc..  It is assumed that the file does not contain duplicates and that the id's x1,x2 etc
	are positive integers which when sorted contain no gaps (i.e. 1 2 3  instead of 1 2 7).
	Also, it is not allowed to have edges returning to the same node.*/
    int loadEdgeData( char* file );

    /** Perform Monte Carlo simulation to estimate the reliability of the network between two nodes.
    Takes t as an optional argument which is the number of iterations to calculate. */
    float estReliabilityMC( int n1, int n2, int t=1000 );
    float getVariance() {return varianceOfLastReliabilitySimulation;};


    /** Prints the edge-data in raw format. */
    void printEdges();

    graph() {};
    ~graph();

private:

	/** Helper function for estReliabilityMC, contains the recursion.
		nc is the current node, and nf is the target. */
	bool unfoldGraph( int nc, int nf, std::vector<edge*> *connectingEdges, bool *visitedNodes );

    int biggestNodeId;

    std::vector<edge*> *connectingEdges; 	//!< Array of lists of edge*, arranged after nodes
	std::vector<edge*> edges;				//!< All edge*s

    float varianceOfLastReliabilitySimulation;
    MTRand randomNbrGenerator;
};




/** The basic node in a network. */
class node
{
public:
    int connect( node *p );
    int getID() {return id;};
    void printConn();

    node( int _maxNeighbors=10 );
    ~node( );

private:
    int id;
    int neighbors,maxNeighbors;
    node **neighbArr;
    static int gid;

};


#endif
