
CC = g++
CFLAGS = -Wall
OBJECTS = graph.o main.o

main : $(OBJECTS)



graph.o : graph.h

clean:
	rm -f $(OBJECTS) main
