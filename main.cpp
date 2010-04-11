/** @file main.cpp

	Main program that calls other functions such as setting up the network and performing simulations.

	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php


 */

#include <iostream>
#include <cstring>
#include <string>
#include "graph.h"
#include "misc.h"


int main(int argv, char **argc)
{
	sArgs args = parseArguments( argv, argc );

	Graph network;
	if ( args.rawFormat == false )
		std::cout << "Trying to load "<< args.filename << std::endl;

	/** Try loading the network from file */
	if ( network.loadEdgeData( args.filename.c_str(), args.rawFormat ) == NO_ERROR )
	{
		// Do we want to perform tests on our algorithm?
		if ( args.mode == PERFORM_TESTS )
		{
			// We have chosen to perform tests on our simulations
			std::cout
				<< "************************************\n"
				<< "   Performing test of 1 cell\n"
				<< "************************************\n";

			if ( network.estReliabilityMC( 1e4, args.rawFormat ) != NO_ERROR )
				std::cout << "Something went wrong in the reliability estimation\n";
			std::cout << "Correct value should be ...\n";

		}	// End of testing
		else if ( args.mode == PERCOLATION )
		{
			std::cout << "Starting percolation calculations for file " << args.filename.c_str() <<std::endl;
			doPercolationCalculation(&network);

		}
		else if ( args.mode == FIND_OPTIMAL_ACO )
		{
			int result = acoFindOptimal(&network, args.Nmax, args.nbrAnts,  args.maxCost);

			std::cout << "ACO returned "<<result<<std::endl;
		}
		else
		{
			// Normal calculation
			std::cout << "Starting normal calculation of " << args.filename.c_str() << std::endl;
			float rel = network.estReliabilityMC( 100000, args.rawFormat );
			if ( rel < 0 )
					std::cout << "Something went wrong in the reliability estimation\n";
			else std::cout << "Reliability = " << rel << std::endl;
		}
	}

	network.finalCleanup();

	return 0;
}
