#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"

//A function that is used to create a new AdjListNode
struct AdjListNode* createAdjListNode(int val, int hueristic){
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); //Allocate the space for the new node
    newNode->val = val; //Set the value as the value we want
    newNode->hueristic = hueristic; //Set the hueristic value
    newNode->next = NULL; //Make the pointer to the next value NULL for now
    return newNode;
}

//A function that is used to create a new Graph
struct Graph* createGraph(int size){
    struct Graph* newGraph = (struct Graph*) malloc(sizeof(struct Graph)); //Allocate the space for the graph
    newGraph->size = size; 
    newGraph->array = (struct AdjList*) malloc(sizeof(struct AdjList)); //Allocate the space for the adjacency list array

    //Set all of the arrays as empty by making the heads NULL
    for(int i=0; i < size; i++){
        newGraph->array[i].head = NULL;
    }

    return newGraph;
}

//A function that adds an edge between start->end & end->start
void addEdge(struct Graph* graph, int start, int end){
    struct AdjListNode* newNode = createAdjListNode(end, 0); //Creates a new node with the end as the value
    newNode->next = graph->array[start].head; //Making the new node the first in the AdjList. newNode->AdjListNode1
    graph->array[start].head = newNode; //Moves head of the AdjList to newNode

    //Also have to add an edge from end to start because graph is undirected
    newNode = createAdjListNode(start, 0); //Creates a new node with the start as the value
    newNode->next = graph->array[end].head; //Making the new node the first in the AdjList. newNode->AdjListNode1
    graph->array[end].head = newNode; //Moves head of the AdjList to newNode
}

void printGraph(struct Graph* graph){
    for(int i=0; i < graph->size; i++){
        struct AdjListNode* tempNode = graph->array[i].head; //make a tempNode that is pointing to the head of an array

        printf("\n Adjacency list of vertex %d\n head ", i);
        //While the temp is not NULL, print the value then set tempNode = to the next node in the list
        while (tempNode){
            printf("-> %d", tempNode->val);
            tempNode = tempNode->next;
        }
        printf("\n");
    }
}

struct Graph* CreateExhibitGraph(){
    const int size = 10;
    struct Graph * graph1 = createGraph(size);
    addEdge(graph1, 1, 2);
    addEdge(graph1, 1, 6);
    addEdge(graph1, 6, 7);
    addEdge(graph1, 6, 9);
    addEdge(graph1, 6, 5);
    addEdge(graph1, 4, 5);
    addEdge(graph1, 4, 3);
    addEdge(graph1, 3, 2);
    addEdge(graph1, 2, 8);
    addEdge(graph1, 8, 7);
    addEdge(graph1, 7, 10);
    addEdge(graph1, 10, 9);

    printGraph(graph1);

    return graph1
}
