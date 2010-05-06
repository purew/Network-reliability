#! /usr/bin/python
#
#	Script for finding good parameters for ACO. Saves the result in 
# 	in text files that can be loaded and plotted from the Matlab-script
#	with the same name.
#	
#	Copyright (c) 2010 Anders Bennehag
#	Licensed under the MIT license
#	http://www.opensource.org/licenses/mit-license.php
#
import sys
import subprocess
import re




# Loop over these arguments
K = range(10, 31,10)
Ants = range(10,51,10)
iterations = 5
network = 'data/10_cell_fc.nwk'
cost = '41'
cores = 2;


# Open the files for reading
fmean = open('Rmean.txt','w')
fmax = open('Rmax.txt','w')

i=0
for k in K:
	for a in Ants:
		Rarr = []
		print 'Starting k='+str(k)+' and a='+str(a),
			
		for j in range(0,iterations,cores):
				
			# Just a counter
			i=i+1		
			
			# Run the simulation on both cores
			subproc = []
			for core in range(0,cores):
				sp = subprocess.Popen(['./main', '-f', network, '-aco', str(cost), str(k), str(a)], stdout=subprocess.PIPE)
				
				subproc.append(sp)
				
			for core in range(0,cores):
				subproc[core].wait()
				data = subproc[core].communicate()
				output = data[0]

				# Get the reliability with regexp
				p = re.compile('^All-terminal reliability = (0.\d*)', re.MULTILINE)
				m = re.search(p, output)
				R = float(m.group(1))
				Rarr.append(R)
			
			#print '.',#+str(i),
		Rmean = 0
		Rmax = 0
		for R in Rarr:
			Rmean += R/len(Rarr)
			if R>Rmax:
				Rmax = R
		#print 'For k='+str(k)+', ants='+str(a)+': 
		print 'Rmax = '+str(Rmax)+', Rmean = '+str(Rmean)
		fmean.write(str(Rmean)+' ')
		fmax.write(str(Rmax)+' ')

	fmean.write('\n')
	fmax.write('\n')

fmean.close()
fmax.close()
		
print "Mean values averaged over "+str(iterations*cores)+" iterations"		
