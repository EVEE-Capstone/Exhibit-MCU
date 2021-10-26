// A C Program to demonstrate adjacency list 
// representation of graphs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A structure to represent an adjacency list node
struct AdjListNode
{
    int dest;
    char type[5];
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
void addEdge(struct Graph* graph, int src, int dest, char srcType[5], char destType[5])
{
    // Add an edge from src to dest.  A new node is 
    // added to the adjacency list of src.  The node
    // is added at the beginning
    struct AdjListNode* destNode = newAdjListNode(dest);
    strcpy(destNode->type, srcType);
    destNode->next = graph->array[src].head;
    graph->array[src].head = destNode;

    // Since graph is undirected, add an edge from
    // dest to src also
    struct AdjListNode* srcNode = newAdjListNode(src);
    strcpy(srcNode->type, destType);
    srcNode->next = graph->array[dest].head;
    graph->array[dest].head = srcNode;
}
  
// A utility function to print the adjacency list 
// representation of graph
void printGraph(struct Graph* graph)
{
    int v;

    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n %d (%s) ", v, v, pCrawl->type);
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
    addEdge(graph, 0, 1, "home", "road");
    addEdge(graph, 1, 2, "road", "road");
    addEdge(graph, 1, 4, "road", "work");
    addEdge(graph, 2, 3, "road", "road");
    addEdge(graph, 3, 5, "road", "solr");
    addEdge(graph, 3, 7, "road", "road");
    addEdge(graph, 3, 4, "road", "work");
    addEdge(graph, 4, 8, "work", "road"); 
    addEdge(graph, 5, 6, "solr", "road");
    addEdge(graph, 6, 7, "road", "road");
    addEdge(graph, 6, 11, "road", "supr");
    addEdge(graph, 7, 8, "road", "road");
    addEdge(graph, 7, 10, "road", "road");
    addEdge(graph, 8, 9, "road", "road");
    addEdge(graph, 9, 10, "road", "road");
    addEdge(graph, 10, 11, "road", "supr");

    // print the adjacency list representation of the above graph
    printGraph(graph);
  
    return 0;
}