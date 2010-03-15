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
#include <string>
#include "MersenneTwister.h"

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

	/** Reset the edge to working condition if it has failed. If the node is disabled a hard reset must be used.*/
	void reset();
	/** Reset the edge no matter the current status. */
	void hardReset() { working=1;};
	/** Disable the edge, only a hard reset will make it functional agian. */
	void disable() { working = -1;};

	bool isWorking() {return working==1;};
	void setWorking( bool status=1 ) {if (working != -1) working = status;};

	float getCost() {return cost;};
	void setReliability( double newReliability ) {reliability = newReliability;};
	double getReliability() {return reliability;};

	/** Get the pheromone level on this link. */
	float acoGetTau() {return acoTau;};
	/** Set the pheromone level on this link. */
	float acoSetTau();

	/** Set the probability to choose this link from origin-node. */
	void acoSetPFromNode( int origin, float p) {if (origin==n[0]) acoP[0]=p; else acoP[1]=p;};
	/** Get the probability to choose this link from origin-node. */
	float acoGetPFromNode( int origin ) {if (origin==n[0]) return acoP[0]; else return acoP[1];};

    int n[2];
private:

	int working;	//!< -1 if disabled, 0 if failed and 1 if working

    float cost;
    double reliability;

	float acoTau;		//!< The pheromone level on this link
	/** acoP[i] is the probability of this link to be chosen from node i */
	float acoP[2];
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
	Also, it is not allowed to have edges returning to the same node.

	Return NO_ERROR on success. Loading a new network removes the previous network.
	Optional parameter makes the function quiet unless there's an error. */
    int loadEdgeData( const char* filename, bool quiet=false );

    /** Perform Monte Carlo simulation to estimate the reliability of the network between two nodes.
    Takes t as an optional argument which is the number of iterations to calculate.
    If rawFormat is set to true, no output will be written.
    Returns the estimated reliability. */
    float estReliabilityMC( int t=1000, bool rawFormat=false );

    float getVariance() {return varianceOfLastReliabilitySimulation;};

	/** Change the reliability of all edges */
	void setEdgeReliability( double newReliability );

	/** Disable N*x edges completely, removing them from all calculations until a hard reset has been done. */
	void disableXEdges( float x );

	/** Reset ALL edges to working condition. Like  a hard reset. */
	void hardResetEdges();

    /** Prints the edge-data in raw format. */
    void printEdges();

    /** Perform percolation calculation and save the results to data/percolation.plot
		For plotting with Matlab. Data is saved in a matrix where increasing columns are
		increasing p and increasing rows are increasing x.*/
    void doPercolationCalculation();

	/****************************************
			ACO-functions
	 ****************************************/

    /** Use ACO to find a near-optimal solution that maximizes reliability
	given a cost restraint of Cmax. */
    int acoFindOptimal( int Nmax, int Cmax, int ants=10 );
	/** Calculate the prob. p that an ant will choose destination from the origin-node. */
	float acoP( int origin, int destination );


    graph();
    ~graph();

private:

	/** Helper function for estReliabilityMC, contains the recursion.
		nc is the current node, and nf is the target. */
	bool unfoldGraph( int nc, int nf, std::vector<edge*> *connectingEdges, bool *visitedNodes );

    int biggestNodeId;	//!< Used for keeping track of the nodes (TODO, a vector would be better)

	void cleanup();		//!< Perform cleanup when done using the graph. Called internally in destructor and load-func.

    std::vector<edge*> *connectingEdges; 	//!< Array of lists of edge*, arranged after nodes
	std::vector<edge*> edges;				//!< All edge*s

	int n1,n2;			//!< Calculate reliability between these two nodes

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
