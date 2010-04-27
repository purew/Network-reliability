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

#include <math.h>
//#include "databank.h"
#include "graphTools.h"
#include "binaryTree.h"

int findDegeneracy(int N, int l)
{
	node* tree=0;

	// Start the real algorithm

	graph *V = initGraph(N,l);
	int D=0;

	do {
		char *str = printGraph( V );
		if (isSymmetric(V, &tree) )
		{
			//printf("%s is symmetric ", str);
			continue;
		}

		// If we reached this point, this network is unique and we should count it
		D += 1;

		/*printf("  databank: ");
		for (int i=0; i < db->index; ++i )
			printf("%i ",db->A[i]);
		printf( "%s hash=%i\n",str, readAsInt(V,0,1 ));*/
		free(str);
	}
	while ( nextPerm( V )==0 );

	deleteTree(tree);
	removeGraph( V );

	return D;
}

int main( int argc, char** argv )
{
	if (argc<2 || argc>3)
	{
		printf("Use the following syntax:\n");
		printf("    degeneracyCounter <number of nodes> [<number of links>]\n");
		printf("    number of links is optional, and if not defined or set to -1, D is calculated for all l's\n");
		return 0;
	}
	int N = atoi( argv[1] );
	if (N>Nmax)
	{
		N = Nmax;
		printf("N is bigger than %i which is the largest supported number\n    Resetting to %i\n",Nmax,Nmax);
	}
	int l = -1;
	if (argc == 3)
		l = atoi( argv[2] );

	if ( l==-1 )
	{
		for ( int k=0; k <= N*(N-1)/2; ++k )
		{
			int D = findDegeneracy(N,k);
			printf("%i %i %i\n", N,k,D);
		}
	}
	else
	{
		int D = findDegeneracy(N,l);
		printf( "The degeneracy for a %i-node network with %i links is %i\n",N,l,D);
	}


	return 0;
};
