#include <stdio.h>
#include <stdlib.h>
 
// Define the maximum number of vertices in the graph
#define N 12
 
// Data structure to store a graph object
struct Graph
{
    // An array of pointers to Node to represent an adjacency list
    struct Node* head[N];
};
 
// Data structure to store adjacency list nodes of the graph
struct Node
{
    int dest;
    struct Node* next;
};
 
// Data structure to store a graph edge
struct Edge {
    int src, dest;
};
 
// Function to create an adjacency list from specified edges
struct Graph* createGraph(struct Edge edges[], int n)
{
    unsigned i;
 
    // allocate storage for the graph data structure
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
 
    // initialize head pointer for all vertices
    for (i = 0; i < N; i++) {
        graph->head[i] = NULL;
    }
 
    // add edges to the directed graph one by one
    for (i = 0; i < n; i++)
    {
        // get the source and destination vertex
        int src = edges[i].src;
        int dest = edges[i].dest;
 
        // 1. allocate a new node of adjacency list from `src` to `dest`
 
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = dest;
 
        // point new node to the current head
        newNode->next = graph->head[src];
 
        // point head pointer to the new node
        graph->head[src] = newNode;
 
        // 2. allocate a new node of adjacency list from `dest` to `src`
 
        newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = src;
 
        // point new node to the current head
        newNode->next = graph->head[dest];
 
        // change head pointer to point to the new node
        graph->head[dest] = newNode;
    }
 
    return graph;
}
 
// Function to print adjacency list representation of a graph
void printGraph(struct Graph* graph)
{
    int i;
    for (i = 0; i < N; i++)
    {
        // print current vertex and all its neighbors
        struct Node* ptr = graph->head[i];
        while (ptr != NULL)
        {
            printf("(%d —> %d)\t", i, ptr->dest);
            ptr = ptr->next;
        }
 
        printf("\n");
    }
}

// Directed graph implementation in C
int main(void)
{
    // input array containing edges of the graph (as per the above diagram)
    // `(x, y)` pair in the array represents an edge from `x` to `y`
    // struct Edge edges[] =
    // {
    //     {0, 500}, { 0, 1 }, {0,200}, 
    //     { 1, 2 }, {1,0}, 
    //     { 2, 200 }, { 2, 1 }, {2,100}, {2,3}, 
    //     { 3, 2 }, {3,4}, {3,5}, {3,6},
    //     { 4, 3 }, { 4,200}, {4,300},
    //     {5,3}, {5,100}, {5,400},
    //     {6,3},{6,300},{6,400},
    //     {100,2}, {100,5},
    //     {200,0}, {200,2}, {200,4},
    //     {300,4}, {300,6},
    //     {400,5}, {400,6},
    //     {500, 0}
    // };

        struct Edge edges[] =
    {
        {0, 11}, { 0, 1 }, {0,8}, 
        { 1, 2 }, {1,0}, 
        { 2, 8 }, { 2, 1 }, {2,7}, {2,3}, 
        { 3, 2 }, {3,4}, {3,5}, {3,6},
        { 4, 3 }, { 4,8}, {4,9},
        {5,3}, {5,7}, {5,10},
        {6,3},{6,9},{6,10},
        {7,2}, {7,5},
        {8,0}, {8,2}, {8,4},
        {9,4}, {9,6},
        {10,5}, {10,6},
        {11, 0}
    };
 
    // calculate the total number of edges
    int n = sizeof(edges)/sizeof(edges[0]);
 
    // construct a graph from the given edges
    struct Graph *graph = createGraph(edges, n);
 
    // Function to print adjacency list representation of a graph
    printGraph(graph);
 
    return 0;
}


