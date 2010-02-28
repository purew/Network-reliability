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
#include "Core/graphics.h"
#include "Core/GLFT_Font.hpp"
#include "misc.h"


int main(int argv, char **argc)
{
	sArgs args = parseArguments( argv, argc );

	graph network;
	std::cout << "Trying to load "<< args.filename << std::endl;

	// Try loading the file
	if ( network.loadEdgeData( args.filename.c_str() ) == NO_ERROR )
	{
		std::cout << "Estimating reliability...\n";
		if ( network.estReliabilityMC( 100000 ) != NO_ERROR )
			std::cout << "Something went wrong in the reliability estimation\n";


		// Do we want a window app?
		if ( argv >= 3 && strcmp(argc[2], "-window" )==0 )
		{
			rendererAllmighty renderer;
			renderer.initWindow(800,600,24,GLFW_WINDOW, "Network Reliability Simulator");
			renderer.setOrtho();
			GLFT_Font normalFont("binreg.ttf",24);
			std::cout << "Entering main-loop\n";
			while (!glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED ))
			{
				normalFont.drawText(10,10, "Testar att printa");

				renderer.swapBuffers();

			}
		}
	}

	return 0;
}
