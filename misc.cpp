
#include <cstring>
#include <iostream>
#include "misc.h"

MTRand randomNbrGenerator;

/** Handle the command line arguments and return a struct with all options
*/
sArgs parseArguments( int argv, char** argc )
{
	sArgs args;
	args.mode = CALCULATE_RELIABILITY_MC;
	args.rawFormat = false;

	for ( int i=0; i<argv; ++i )
	{
		if ( strcmp(argc[i], "--file")==0  || strcmp(argc[i], "-f")==0 )
		{
			if (i+1<argv)
				args.filename = argc[i+1];
		}
		else if ( strcmp(argc[i], "--mode")==0 || strcmp(argc[i], "-m") == 0 )
		{
			if ( i+1<argv )
			{
				if ( strcmp(argc[i+1], "test")==0 )
					args.mode = DESIGN_NEW_NETWORK;

				else if ( strcmp(argc[i+1], "percolation")==0 )
					args.mode = PERCOLATION;

				else if ( strcmp(argc[i+1], "find_optimal_aco")==0 )
					args.mode = FIND_OPTIMAL_ACO;

				else std::cout << "ERROR: "<<argc[i+1]<<" is not a valid mode\n";
			}
			else std::cout << "A mode must follow --mode, examples \"test\" or \"percolation\"\n";
		}
		else if ( strcmp(argc[i], "--raw")==0 || strcmp(argc[i], "-r")==0 )
		{
			args.rawFormat = true;
		}

	}

	// Handle defaults
	if ( args.mode == PERFORM_TESTS )
		args.filename = "data/1_cell.nwk";

	if ( args.filename.size() == 0 )
	{
		if ( args.rawFormat == false )
			std::cout << "No network-layout supplied, loading default\n";
		args.filename = "data/1_cell.nwk";
	}



	return args;
}
