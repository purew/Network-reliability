/**
	This program's purpose is to generate all networks for a given N.
	We want to count the degeneracy for different numbers of links L.

	We will iterate over all L, and for each L, we will generate all
	permutations of the network, check if each permutation has
	appeared before (symmetric and/or mirrored). If not, we count it as one.

	Take care to notice that the way we check if a network has appeared
	before takes advantage of the network being encoded as an integer
	which serves as a unique identifier. This means that we cannot
	measure networks larger than 11 nodes since more nodes means more
	links which means that we cannot encode the network in a long long.

	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
*/

#ifndef GRAPHTOOLS_H_
#define GRAPHTOOLS_H_

#include <math.h>
#include "mt64/mt64.h"
#include "binaryTree.h"

/** This number CANNOT be changed. The hash technique in this algorithm
	uses the fact that an unsigned long long can hold integers as large
	as 18,446,744,073,709,551,615. Therefore, networks with more than
	11 nodes cannot be examined (unless we switch to another data-type) */
static const int Nmax = 11;

static const int asciiOffset = 48; // Offset to '0' in ascii


/** Struct to hold information about the network */
typedef struct
{
	char *data;
	int length;
	int nbrNodes;
} graph;

/** Generate a simple network consisting of N nodes and L links. Delete with removeGraph. */
graph* initGraph( int N, int L );
/** Frees all data allocated by the graph.*/
void removeGraph( graph *V );

/** Plot the binary vector that identifies a graph.*/
char *printGraph( graph* V );

/** Sum all the numbers up to and including N.*/
int sumUpTo( int N );
/** Return wether node i and j is connected or not. */
char isConnected(graph *V, int i, int j);
void setConnected(graph *V, int i, int j, char connected);

/** Update *V to the next permutation.
	Returns 0 if succesful and 1 otherwise. If unsuccesful, *V is garbage...*/
int nextPerm( graph *V );

/** Treat V as a binary vector and read the encoded integer.*/
long long readAsInt( graph *V );

/** Check if this graph has appeared in some other encoding before.
	We don't want to count duplicates. */
int isSymmetric( graph *V, node **tree );


/** Creates a copy of the network where the network is offsetted, ie. a symmetric copy */
void offsetGraph( graph *V, graph *Vmod, int offset, int dir);

/** Estimate the reliability from Q monte carlo iterations, where each link has reliability p.*/
float estReliability( graph* V, int Q, float p,state64 *rndState);

#endif
