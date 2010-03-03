/** @file misc.h

	Various random snippets of code that doesnt fit elsewhere.

	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
 */

#ifndef MISC_H_
#define MISC_H_

#include <string>


enum modes  {	CALCULATE_RELIABILITY_MC,
				DESIGN_NEW_NETWORK,
				DISPLAY_NETWORK,
				PERCOLATION,
				PERFORM_TESTS,
};

/** A struct containing all the default values and arguments from command line.
	Depends on program. */
struct sArgs
{
	std::string filename;
	modes mode;
	bool rawFormat;
};

/** Parse the arguments, supply default values and return a struct with complete parameters. */
sArgs parseArguments( int argv, char** argc );














#endif
