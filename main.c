
// A C Program to demonstrate adjacency list 
// representation of graphs
#include <stdio.h>
#include <stdlib.h>
  
// A structure to represent an adjacency list node
struct AdjListNode
{
    int dest;
    struct AdjListNode* next;
};
  
// A structure to represent an adjacency list
struct AdjList
{
    struct AdjListNode *head; 
};
  
// A structure to represent a graph. A graph
// is an array of adjacency lists.
// Size of array will be V (number of vertices 
// in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};
  
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest)
{
    struct AdjListNode* newNode =
     (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}
  
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = 
        (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
  
    // Create an array of adjacency lists.  Size of 
    // array will be V
    graph->array = 
      (struct AdjList*) malloc(V * sizeof(struct AdjList));
  
    // Initialize each adjacency list as empty by 
    // making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
  
    return graph;
}
  
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest)
{
    // Add an edge from src to dest.  A new node is 
    // added to the adjacency list of src.  The node
    // is added at the beginning
    struct AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
  
    // Since graph is undirected, add an edge from
    // dest to src also
    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}
  
// A utility function to print the adjacency list 
// representation of graph
void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n head ", v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}
  
// Driver program to test above functions
int main()
{
    // create the graph given in above fugure
    int V = 12;
    struct Graph* graph = createGraph(V);

    addEdge(graph, 0, 1);
    addEdge(graph, 0, 200);
    addEdge(graph, 0, 500);
    addEdge(graph, 1, 2);
    addEdge(graph, 2, 200);
    addEdge(graph, 2, 100);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);
    addEdge(graph, 3, 5);
    addEdge(graph, 3, 6);
    addEdge(graph, 4, 200);
    addEdge(graph, 4, 300);
    addEdge(graph, 5, 100);
    addEdge(graph, 5, 400);
    addEdge(graph, 6, 400);
    addEdge(graph, 6, 300);
  
    // print the adjacency list representation of the above graph
    printGraph(graph);
}
// int main(void){
//     const int size = 10;
//     struct Graph * graph1 = createGraph(size);
//     addEdge(graph1, 1, 2);
//     addEdge(graph1, 1, 6);
//     addEdge(graph1, 6, 7);
//     addEdge(graph1, 6, 9);
//     addEdge(graph1, 6, 5);
//     addEdge(graph1, 4, 5);
//     addEdge(graph1, 4, 3);
//     addEdge(graph1, 3, 2);
//     addEdge(graph1, 2, 8);
//     addEdge(graph1, 8, 7);
//     addEdge(graph1, 7, 10);
//     addEdge(graph1, 10, 9);

//     printGraph(graph1);

//     return 0;
// }