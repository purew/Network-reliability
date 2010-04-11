
#include <cstring>
#include <iostream>
#include <cstdlib>
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
		else if ( strcmp(argc[i], "--aco") == 0 )
		{
			if ( i+1<argv )
			{
				args.mode = FIND_OPTIMAL_ACO;
				args.maxCost=atoi( argc[i+1] );
				if ( args.maxCost == 0)
/*					std::cout << "************************************\n"
						"    No maxCost specified for ACO.\n"
						"**********************************\n";*/
					std::cout << "ERROR: maxCost was not chosen\n";
			}
			else std::cout << "ERROR: Must specify maxCost after --aco\n";

			if ( i+2<argv )
			{
				args.Nmax=atoi( argc[i+2] );
				if ( args.Nmax == 0)
					args.Nmax = 10;
			}
			else args.Nmax = 10;

			if ( i+3<argv )
			{
				args.nbrAnts=atoi( argc[i+3] );
				if ( args.nbrAnts == 0)
					args.nbrAnts = 3;
			}
			else args.nbrAnts = 3;

		}
		else if ( strcmp(argc[i], "--percolation") == 0 )
			args.mode = PERCOLATION;
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
