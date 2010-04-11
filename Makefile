#	Copyright (c) 2010 Anders Bennehag
#	Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php

CC 		= g++
CXXFLAGS= -Wall -O3
CFLAGS 	= $(CXXFLAGS)
LDFLAGS+=

DEPS 	= graph.h misc.h ants.h
SOURCES	= graph.cpp main.cpp misc.cpp ants.cpp
OBJECTS = graph.o   misc.o main.o ants.o


# Make the object-files depend on the c- and h-files
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

# Create the main executable
main 		: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Create profiled version
profile 		: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) -pg $(LDFLAGS)


# Prevent make from touching a file named clean
.PHONY: clean

clean:
	rm -f $(OBJECTS) main

