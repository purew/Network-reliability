/**
	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
*/


#include "graphTools.h"




/** Generate a simple network consisting of N nodes and L links. */
graph* initGraph( int N, int L )
{
	if (N>Nmax)
	{
		printf("Sorry, we currently don't support more than %i nodes\n", Nmax );
		return 0;
	}
	graph* V = malloc( sizeof(graph) );
	V->length = N*(N-1)/2;
	V->data = malloc( sizeof(char) * V->length );
	V->nbrNodes = N;

	// Place the L links in the beginning of the vector
	for ( int i=0; i<V->length; ++i )
	{
		if ( i<L )
			V->data[i] = 1;
		else
			V->data[i] = 0;
	}
	return V;
}
void removeGraph( graph *V )
{
	free( V->data );
	free( V );
}
/** Plot the binary vector that identifies a graph.*/
char *printGraph( graph* V )
{
	char *str = malloc( sizeof(char)*(V->length+1) );
	for ( int i=0; i < V->length; ++i )
	{
		str[i] =  V->data[i]+asciiOffset;
	}
	str[V->length] = '\0';
	return str;
}

/** Sum all the numbers up to and including N.*/
inline int sumUpTo( int N )
{
	return  (N+1)*N/2;
}

/** Private function for the inner workings of both isConnected and setConnected.
	Returns a pointer to the element corresponding to the link between node i and j.*/
char *privNodesConnected(graph *V, int i, int j)
{
	int nbrNodes = V->nbrNodes;
	i = (i+ V->nbrNodes) % V->nbrNodes;
	j = (j+ V->nbrNodes) % V->nbrNodes;
	if (i>=j)
	{
		if (i==j)
		{
			printf("privNodesConnected: i==j SHOULD NEVER HAPPEN\n");
			return 0;
		}
		else
		{
			int temp = j;
			j = i;
			i = temp;
		}
	}

	int index = i*V->nbrNodes-sumUpTo(i+1)+j;
	if ( index >= V->length )
	{
		printf( "ERROR: isConnected(): index >= V->length\n" );
		return 0;
	}
	return &(V->data[index]);
}
char isConnected(graph *V, int i, int j)
{
	char *isConn = privNodesConnected( V,i,j );
	return *isConn;
}
void setConnected(graph *V, int i, int j, char connected)
{
	char *isConn = privNodesConnected( V,i,j );
	*isConn = connected;
}

/** Update *V to the next permutation.
	Returns 0 if succesful and 1 otherwise. If unsuccesful, *V is garbage...*/
int nextPerm( graph *V )
{
	char *D = V->data;
	int len = V->length;

	int i = len-1;
	int onesRemoved = 0;

	// First, start from the right and remove all 1's until a 0 is encountered
	while ( D[i]==1 )
	{
		D[i] = 0;
		--i;
		++onesRemoved;
		if ( i<0 )  // Shouldn't happen
			return 1;
	}

	// Then, keep going until the next 1 is encountered
	while ( D[i]==0 )
	{
		--i;
		if ( i<0 ) // If this happens, there are no more permutations
			return 1;
	}

	// The next permutation is achieved by moving this 1 one step and then placing
	// all of the earlier removed 1's after this one
	D[i] = 0;
	++onesRemoved;

	// Put out the earlier removed 1's
	for ( int k=0 ; k<onesRemoved; ++k )
	{
		D[i+1+k] = 1;
	}

	return 0;
}

/** Treat V as a binary vector and read the encoded integer.
		This will be a unique hash-value. We have to do this for different
		starting positions and both backwards and forwards in order to cover
		symmetric and mirrored networks.
		dir is either -1 or +1, depending on intended direction. */
long long readAsInt( graph *V )
{
	long long Q = 0;
	for ( int i=0; i < V->length; ++i )
	{
		if ( V->data[i] == 1 )
			Q += pow( 2, i );
	}
	return Q;
}

/** Check if this graph has appeared in some other encoding before. */
int isSymmetric( graph *V, node **tree )
{
	// We first search through all networks that are symmetric and find the
	// smallest id. This id is checked against the database

	// TODO: We can only deal with rotational symmetries atm
	// 		 Reflected symmetries requires an additional nested for-loop

	// Create our vector that will be offsetted and reflected
	graph *Vmod = malloc( sizeof(*Vmod) );
	Vmod->length = V->length;
	Vmod->nbrNodes=V->nbrNodes;
	Vmod->data = malloc( sizeof(*(Vmod->data))*Vmod->length);

	long long minHash=readAsInt( V );

	// Offset the links in the network: link 1-2 becomes 2-3 etc
	for (int i=0;i < V->nbrNodes; ++i)
		// We cannot detect rotational symmetries yet
		// And does this way even work?

		//	printf("i=%i, dir=%i V-nodes %i, Vmod-nodes %i\n", i,dir,V->nbrNodes, Vmod->nbrNodes);
		offsetGraph(V, Vmod, i, 1);
		//printf("modified: %s\n",printGraph(Vmod));
		long long hash = readAsInt( Vmod );
		if ( hash < minHash )
		{
			minHash = hash;

		}
	if ( hasNum(*tree, minHash) )
		return 1;
	else
	{
//		printf("V=%s, new minHash=%i\n",printGraph(Vmod),minHash);
//		printf( "offset %i, dir %i\n", offset, dir);
//		printf("%s\n%s\n",printGraph(V),printGraph(Vmod));

		addValue( tree, minHash );
		return 0;
	}
}


void offsetGraph( graph *V, graph *Vmod, int offset, int dir)
{
	// TODO: We can only deal with rotational symmetries atm
	// 		 Reflected symmetries requires an additional nested for-loop
	// The current dir, makes no difference, if we do + or - offset
	// checks the same numbers...
//	if (Vmod->nbrNodes == 0)
//		printf("V-nodes %i, Vmod-nodes %i\n", V->nbrNodes, Vmod->nbrNodes);
	for (int i=0; i < V->nbrNodes-1; ++i)
		for (int j=i+1; j < V->nbrNodes; ++j)
		{
			int ijConn = isConnected(V,i,j);
			setConnected( Vmod, i+offset, j+offset,ijConn);
			/*if (dir == 1)

			else	// Backwards
				setConnected( Vmod, i-offset,j-offset,ijConn);*/
		}
	return;
}
