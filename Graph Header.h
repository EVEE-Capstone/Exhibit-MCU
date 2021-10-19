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
struct AdjListNode* CreateAdjListNode(int val){
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); //Allocate the space for the new node
    newNode->val = val; //Set the value as the value we want
    newNode->next = NULL; //Make the pointer to the next value NULL for now
    return newNode;
}

//A function that is used to create a new Graph
struct Graph* CreateGraph(int size){
    struct Graph* newGraph = (struct Graph*) malloc(sizeof(struct Graph)); //Allocate the space for the graph
    newGraph->size = size; 
    newGraph->array = (struct AdjList*) malloc(sizeof(size * struct AdjList)); //Allocate the space for the adjacency list array

    //Set all of the arrays as empty by making the heads NULL
    for(int i=0; i < size; i++){
        graph->array[i].head = NULL;
    }

    return newGraph;
}

//A function that adds an edge between start->end & end->start
void addEdge(struct Graph* graph, int start, int end){
    struct AdjListNode* newNode = CreateAdjListNode(end); //Creates a new node with the end as the value
    newNode->next = graph->array[start].head; //Making the new node the first in the AdjList. newNode->AdjListNode1
    graph->array[start].head = newNode; //Moves head of the AdjList to newNode

    //Also have to add an edge from end to start because graph is undirected
    newNode = CreateAdjListNode(start); //Creates a new node with the start as the value
    newNode->next = graph->array[end].head; //Making the new node the first in the AdjList. newNode->AdjListNode1
    graph->array[end].head = newNode; //Moves head of the AdjList to newNode
}

void PrintGraph(struct Graph* graph){
    for(int i=0, i < graph->size; i++){
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
