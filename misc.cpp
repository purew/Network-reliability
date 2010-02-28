
#include <cstring>
#include <iostream>
#include "misc.h"




sArgs parseArguments( int argv, char** argc )
{
	sArgs args;
	args.mode = CALCULATE_RELIABILITY_MC;

	for ( int i=0; i<argv; ++i )
	{
		if ( strcmp(argc[i], "-o")==0 )
		{
			if (i+1<argv)
				args.filename = argc[i+1];
		}
		else if ( strcmp(argc[i], "--design")==0 )
		{
			args.mode = DESIGN_NEW_NETWORK;
		}


	}

	// Handle defaults
	if ( args.filename.size() == 0 )
	{
		std::cout << "No network-layout supplied, loading default\n";
		args.filename = "data/1_cell.nwk";
	}

	return args;
}
