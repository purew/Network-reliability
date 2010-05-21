/*
	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
*/

#include "ants.h"
#include "graph.h"


Ant::Ant(int maxLinks)
{
	workingLinks = new int[ maxLinks ];
	for (int i=0; i<maxLinks; ++i)
		workingLinks[i] = 0;


	Graph();
}

Ant::~Ant()
{
	delete[] workingLinks;
}
