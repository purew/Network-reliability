/** @file graph.h
 *
 * Classes for handling the structure of the network. They are assumed
 * to be static once created but this assumption is purely for faster
 * execution and simplicity of code.
 */


#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <list>

enum errors {   NO_ERROR=0,
                DUPLICATE,
                MAX_NEIGHBORS,
                FILE_OPEN_ERROR,
};


/** A class representing the edge in our graph.
    The edge goes both ways. */
class edge
{
public:
    edge( int n1, int n2, float reliability = 0.8, float cost=1.0 );
    const int* getNodes() { return n; };

private:
    int n[2];

    float cost;
    float reliability;
};

/** Class for keeping track of the graph.
    Performs loading, cleanup, etc. */
class graph
{
public:
    int loadEdgeData( char* file );

private:
    std::list<edge> edges;
};




/** The basic node in a network. */
class node
{
public:
    int connect( node *p );
    int getID() {return id;};
    void printConn();

    node( int _maxNeighbors=10 );
    ~node( );

private:
    int id;
    int neighbors,maxNeighbors;
    node **neighbArr;
    static int gid;

};


#endif
