/** @file main.cpp

	Main program that calls other functions ie. sets up
	the network and performs simulations.

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

	graph network;
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
			// In percolation mode, we want to calculate R(x,p) where N*x=F are removed
			// from the node immediately. p is the normal reliability per edge.
			float stepSizeP = 0.01;
			float stepSizeX = 0.0476;
			for ( float x=0; x<=1.0; x+=stepSizeX)
			{

				// Disable N*x edges that will not take part of the simulation
				network.disableXEdges(x);

				//std::cout << "*******Failed edges: "<<network.nbrFailed()<<std::endl;

				for ( float p=0; p<=1.0; p+=stepSizeP )
				{
					//std::cout << p << " " << x << " ";
					network.setEdgeReliability( p );
					if ( network.estReliabilityMC( 4e3, args.rawFormat ) != NO_ERROR )
						std::cout << "Something went wrong in the reliability estimation\n";
				}
				std::cout << std::endl;

				// Restore the disabled edges
				network.hardResetEdges();
			}

		}
		else
		{
			// Normal calculation
			if ( network.estReliabilityMC( 100000, args.rawFormat ) != NO_ERROR )
					std::cout << "Something went wrong in the reliability estimation\n";
		}
	}
	return 0;
}
