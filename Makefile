#	Copyright (c) 2010 Anders Bennehag
#	Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php

CC 		= g++
CXXFLAGS= -Wall  `freetype-config --cflags`
CFLAGS 	= $(CXXFLAGS)
LDFLAGS+= -lGL -lglfw -lglpng -lpthread -lGLU `freetype-config --libs`

DEPS = graph.h Core/GLFT_Font.h Core/graphics.h misc.h
OBJECTS = graph.o  Core/GLFT_Font.o Core/graphics.o misc.o main.o


# Make the object-files depend on the c- and h-files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

# Create the main executable
main 		: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Prevent make from touching a file named clean
.PHONY: clean

clean:
	rm -f $(OBJECTS) main

