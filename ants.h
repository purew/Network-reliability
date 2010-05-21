/*
	Copyright (c) 2010 Anders Bennehag
	Licensed under the MIT license
	http://www.opensource.org/licenses/mit-license.php
*/

#ifndef ANTS_H_
#define ANTS_H_

#include <vector>
#include "graph.h"


/** The ant-class is designed for keeping track of the path, cost and reliability
	of an ant in the network. */
class Ant : public Graph
{
public:
	/** Set the working level of one of the links. */
	void setLinkLevel( int link, int level ) {workingLinks[link] = level;};
	int getLinkLevel(int link) {return workingLinks[link];};


	/** Constructor takes the maximum number of links in the network as argument. */
	Ant( int maxLinks );
	~Ant();

private:
	/** An array mapping each link to non-working or working level. */
	int *workingLinks;

};





#endif
