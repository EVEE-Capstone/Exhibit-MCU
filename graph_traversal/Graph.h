//All of the functions to create a graph and all of the structs needed

#include <stdio.h>
#include <stdlib.h>

//Represents the NODES that make up the AdjList
struct AdjListNode{
    int val; // value at that node
    struct AdjListNode* next; //next node in the AdjList
};

//Represents the list of Adjacent nodes 
struct AdjList{
    struct AdjListNode *head; //pointer to the first element of the AdjList
};

//Represents the graph as a whole
struct Graph{
    int size; //max number of vertices in the AdjList
    struct AdjList* array; //An array of all the AdjList made. 
};

//A function that is used to create a new AdjListNode
struct AdjListNode* CreateAdjListNode(int val);
//A function that is used to create a new Graph
struct Graph* CreateGraph(int size);

//A function that adds an edge between start->end & end->start
void addEdge(struct Graph* graph, int start, int end);

void PrintGraph(struct Graph* graph);
