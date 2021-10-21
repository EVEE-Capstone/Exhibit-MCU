#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct AdjListNode
{
    char *dest;
    struct AdjListNode* next;
};

struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};

struct Graph
{
    int V;
    struct AdjList* array;
};

struct AdjListNode* newAdjListNode(char *dest)
{
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;

    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

    // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

void addEdge(struct Graph* graph, char *src, char *dest)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the beginin

    struct AdjListNode* newNode = newAdjListNode(dest);

    //***Changed. you need to add edge in src node. But you were always adding in 0
    newNode->next = graph->array[src[0]-'a'].head;
    graph->array[src[0]-'a'].head = newNode;

    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src);

    //***Changed. you need to add edge in dest node. But you were always adding in 1
    newNode->next = graph->array[dest[0]-'a'].head;
    graph->array[dest[0]-'a'].head = newNode;
}

void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n head %c", v, v + 'a');
        while (pCrawl)
        {
            printf("-> %s", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

int main()
{
    // create the graph given in above fugure
    int V = 5;
    struct Graph* graph = createGraph(V);
    addEdge(graph, "Home", "1");
    addEdge(graph, "1", "2");
    addEdge(graph, "1", "6");
    addEdge(graph, "2", "3");
    addEdge(graph, "2", "8");
    addEdge(graph, "4", "5");
    addEdge(graph, "5", "6");
    addEdge(graph, "6", "9");
    addEdge(graph, "6", "7");
    addEdge(graph, "7", "8" );
    addEdge(graph, "7", "Destination Charging");
    addEdge(graph, "8", "Super Charger");
    addEdge(graph, "9", "10");
    addEdge(graph, "10", "Destination Charging");


    // print the adjacency list representation of the above graph
    printGraph(graph);

    return 0;
}
