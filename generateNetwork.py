#! /usr/bin/python
#
#	Generates a network and saves into data-files (.nwk)
#	Used with the following syntax:
#		./generateNetwork.py <type> <cells wide> <cells high> <rotations> <filename> [<fully connected=False>]
#			<type> is chain, or ring
#
#	Copyright (c) 2010 Anders Bennehag
#	Licensed under the MIT license
#	http://www.opensource.org/licenses/mit-license.php
#

# TODO make_ring creates duplicate south links when connecting the sides

import sys

def save_network( network, filename, options):
	
	f = open(filename, 'w')
	# Header. prob is the probability that each link will fail
	f.write("type: edges\nprob: 0.6\n")
	f.write("# "+options[0]+"\n# "+str(options[1])+ " cells wide \n# "+str(options[2])+" cells high \n# "+str(options[3])+" rotation \n# Fully connected: "+str(options[4])+"\n")	
	for n1,n2 in network:
		f.write( str(n1)+" "+str(n2)+"\n" )
	f.close()

def make_chain(width, height, fully_connected=False):
	# The network begins in the lower left corner like the following 6 node chain:
	# 3 4 5
	# 0 1 2 
	
	network = []
	for n in range(width*height):
		# Go through all the nodes and add the w, sw, s and, se links to the network
		first_col = n%width ==0
		last_col = (n+1)%width ==0
		
		if not first_col: # Make sure we're not in the first column
			network.append([n-1,n]) # west
			
		if n/width>0: # Make sure we're not in the first row
			
			if not first_col: # Not in first column again				
				network.append([n-width-1,n]) # south west
			
			if fully_connected or first_col or last_col:
				network.append([n-width,n]) # south	
			
			if not last_col: # Not in last column
				network.append([n,n-width+1]) # south east
	return network
	
def make_cylinder(width, height, rot, fully_connected=False):
	# The network begins in the lower left corner like the following 6 node chain:
	# 3 4 5
	# 0 1 2 
	def trfm( n ):
		
		# All node id's must be offsetted after the first
		# row if creating a ring. For example, if width=3, node 3
		# will be transformed into node 0, node 4 must then be offsetted
		# to be the new node 3
		offset = n/width
		
		# Function for substituting the last nodes into the id's of the 
		# first columns, thereby creating a ring or cylinder from a chain
		if (n+1)%width==0: # Are we in the last column?
			# What we want to do now depends on if we want rotation of not
			if not rot:
				return n-(width-1)-offset
			else:
				row = (n-(width-1))/width
				nn =(height-1-row)*width
				offset = nn/width
				nn = nn-offset
				print n,row,nn
				return nn
		else:
			return n-offset
			
	
	network = []
	w = width
	for n in range(width*height):
		first_col = n%width ==0
		last_col = (n+1)%width ==0
		# Go through all the nodes and add the w, sw, and s links to the network
		# Since it's a ring, nodes connected to the last column will in reality be connected to the first column

		if not first_col: # Make sure we're not in the first column
			network.append([trfm(n-1),trfm(n)]) # west
		if n/width>0: # Make sure we're not in the first row
			
			if not first_col: # Not in first column again				
				network.append([trfm(n-width-1),trfm(n)]) # south west
			
			if first_col or (not last_col and fully_connected):
				network.append([trfm(n-width),trfm(n)]) # south	
			
			if (n+1)%width!=0: # Not in last column
				network.append([trfm(n),trfm(n-width+1)]) # south east
	return network

if __name__ == '__main__':
	
	type = "uninitialized"
	# (Default) Properties of the chain
	width = 4
	# Properties of the ring
	rotations = 0
	# Properties of the cylinder
	height = 1
	# Properties of the toroid
	
	if len(sys.argv)==1:
		print """
	Generates a network and saves into data-files (.nwk)
	Used with the following syntax:
		./generateNetwork.py <type> <cells wide> <cells high> 
			<rotations> <filename> [<fully connected=False>]
			
			<type> is chain, or ring"""
	else:
		
		filename = "data/generated"
		fully_connected = False
		
		if len(sys.argv)>1:
			if sys.argv[1] == "chain":
				type = sys.argv[1]
			elif sys.argv[1] == "ring" or sys.argv[1] == "cylinder":
				type = "cylinder"
			elif sys.argv[1] == "toroid":
				type = "toroid"
			else:
				type = "chain"
				 
		if len(sys.argv)>2:
			width = int(sys.argv[2])+1
		if len(sys.argv)>3:
			height = int(sys.argv[3])+1
		if len(sys.argv)>4:
			rotations = int(sys.argv[4])
		if len(sys.argv)>5:
			filename = sys.argv[5]
		if len(sys.argv)>6:
			fully_connected = sys.argv[6]=="FC" or sys.argv[6]=="fc"
			
			
			
		cells_wide = width-1
		cells_high = height-1
		print "Generating a",cells_wide,"cell wide",type,"with",rotations,"rotations and",cells_high, "cells high"
		if fully_connected:
			print "Network is fully connected:"
		else:
			print "Network is not fully connected:"

		if type=="chain":
			network = make_chain(width, height, fully_connected)
		elif type=="cylinder":
			network = make_cylinder(width, height, rotations, fully_connected)
		
		print network
		print "Size is",len(network)
		
		save_network(network, filename, [type, cells_wide, cells_high, rotations,fully_connected])
