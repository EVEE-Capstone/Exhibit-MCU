#include <stdio.h>
#include <stdlib.h>

//Represents the NODES that make up the AdjList
struct AdjListNode{
    int val; // value at that node
    int hueristic; //Hueristic value for the node
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
struct AdjListNode* createAdjListNode(int val, int hueristic);
//A function that is used to create a new Graph
struct Graph* createGraph(int size);

//A function that adds an edge between start->end & end->start
void addEdge(struct Graph* graph, int start, int end);

void printGraph(struct Graph* graph);
