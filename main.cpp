/** @file main.cpp

	Main program that calls other functions ie. sets up
	the network and performs simulations.

	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php


 */

#include <iostream>
#include "graph.h"
#include "Core/graphics.h"
#include "Core/GLFT_Font.hpp"



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
		rendererAllmighty renderer;
		renderer.initWindow(800,600,24,GLFW_WINDOW, "CORE");
		renderer.setOrtho();
		GLFT_Font normalFont("binreg.ttf",24);
		std::cout << "Entering main-loop\n";
		while (!glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED ))
		{
			normalFont.drawText(10,10, "Testar att printa");

			renderer.swapBuffers();

		}



		network.estReliabilityMC(1,7, 10000 );
	}
	else
	{
		std::cout << "Problem with loading of file\n";
	}
	return 0;
}
