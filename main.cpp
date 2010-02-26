/** @file main.cpp

  Main program that calls other functions ie. sets up
  the network and performs simulations.

 */

#include <iostream>
#include "graph.h"



int main(int argv, char **argc)
{
	graph network;
	if (argv == 1)
	{
		std::cout << "Supply a filename to a network file, for example \"data/1_cell.nwk\"\n";
		return 0;
	}
	std::cout << "Trying to load "<< argc[1] << std::endl;

	int gErr = network.loadEdgeData( argc[1] );
	if ( gErr==0 )
	{
		network.estReliabilityMC(1,7 );
	}
	else
	{
		std::cout << "Problem with loading of file\n";
	}
	return 0;
}
