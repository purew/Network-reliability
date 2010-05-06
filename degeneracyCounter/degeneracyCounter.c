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
#include <time.h>
#include "mt64/mt64.h"
#include "graphTools.h"
#include "binaryTree.h"

typedef struct
{
	int D;
	float bestR;
} degeneracy;

degeneracy findDegeneracy(int N, int l, float p, int Q)
{
	node* tree=0;
	state64 rndState;
	init_genrand64( &rndState, time(0));

	// Start the real algorithm

	graph *V = initGraph(N,l);
	degeneracy deg = {0, 0};
	//deg.D=0;
	//deg.bestR = 0;

	do {
		char *str = printGraph( V );
		if (isSymmetric(V, &tree) )
		{
			//printf("%s is symmetric ", str);
			continue;
		}

		// If we reached this point, this network is unique and we should count it
		deg.D += 1;
		float R = estReliability(V,Q, p, &rndState);

		if ( R>deg.bestR )
		{

			deg.bestR = R;
		}
		/*printf("  databank: ");
		for (int i=0; i < db->index; ++i )
			printf("%i ",db->A[i]);
		printf( "%s hash=%i\n",str, readAsInt(V,0,1 ));*/
		free(str);
	}
	while ( nextPerm( V )==0 );

	deleteTree(tree);
	removeGraph( V );

	return deg;
}

int main( int argc, char** argv )
{
	float p = 0.8;
	int l = -1;
	int Q = 100;

	if (argc<2 || argc>5)
	{
		printf("Use the following syntax:\n");
		printf("\t\tdegeneracyCounter\n\t<number of nodes>\n\t[<number of links>]\n\t[<prob. p of failure>]\n\t[<Q>]\n");
		printf("\t\t-number of links is optional, and if not defined or set to -1, D is calculated for all l's\n");
		printf("\t\t-probability p of failure is optional and defaults to %f\n",p);
		printf("\t\t-number of iterations when estimating reliability, defaults to %i\n",Q);
		return 0;
	}
	int N = atoi( argv[1] );
	if (N>Nmax)
	{
		N = Nmax;
		printf("N is bigger than %i which is the largest supported number\n    Resetting to %i\n",Nmax,Nmax);
	}
	if (argc >= 3)
		l = atoi( argv[2] );

	if (argc >= 4)
		p = atof( argv[3] );
	if (argc >= 5)
		Q = atoi( argv[4] );

	if ( l==-1 )
	{
		//printf("N\tl\tD\tRbest\n");
		for ( int k=0; k <= N*(N-1)/2; ++k )
		{
			degeneracy deg = findDegeneracy(N,k, p, Q);
			printf("%i\t%i\t%i\t%f\n", N,k,deg.D, deg.bestR);
		}
	}
	else
	{
		degeneracy deg = findDegeneracy(N,l,p,Q);
		printf( "The degeneracy for a %i-node network with %i links is %i\n",N,l,deg.D);
		printf( "And the best reliability is %f\n", deg.bestR);
	}


	return 0;
};
