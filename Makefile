
CC = g++
CFLAGS = -Wall
OBJECTS = graph.o main.o

main : $(OBJECTS)

graph.o : graph.h graph.cpp

clean:
	rm -f $(OBJECTS) main
