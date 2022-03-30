/***************************************************************************/ /**
                                                                               * @file main_pg12.c
                                                                               * @brief This project reads input on the LEUART RX port and echoes it back to
                                                                               * the TX port one line at a time. It idles in EM2. See readme.txt for details.
                                                                               *******************************************************************************
                                                                               * # License
                                                                               * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
                                                                               *******************************************************************************
                                                                               *
                                                                               * SPDX-License-Identifier: Zlib
                                                                               *
                                                                               * The licensor of this software is Silicon Laboratories Inc.
                                                                               *
                                                                               * This software is provided 'as-is', without any express or implied
                                                                               * warranty. In no event will the authors be held liable for any damages
                                                                               * arising from the use of this software.
                                                                               *
                                                                               * Permission is granted to anyone to use this software for any purpose,
                                                                               * including commercial applications, and to alter it and redistribute it
                                                                               * freely, subject to the following restrictions:
                                                                               *
                                                                               * 1. The origin of this software must not be misrepresented; you must not
                                                                               *    claim that you wrote the original software. If you use this software
                                                                               *    in a product, an acknowledgment in the product documentation would be
                                                                               *    appreciated but is not required.
                                                                               * 2. Altered source versions must be plainly marked as such, and must not be
                                                                               *    misrepresented as being the original software.
                                                                               * 3. This notice may not be removed or altered from any source distribution.
                                                                               *
                                                                               *******************************************************************************
                                                                               * # Evaluation Quality
                                                                               * This code has been minimally tested to ensure that it builds and is suitable
                                                                               * as a demonstration for evaluation purposes only. This code will be maintained
                                                                               * at the sole discretion of Silicon Labs.
                                                                               ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define NUM_VERTICES 19
#define NUM_EDGES 52
#define START_VERTEX 0
#define END_VERTEX 9
#define HOME 1
#define WORK 12
#define OUTAGE 5
#define SOLAR 17
#define INSTRUCTIONS_SIZE 1000
#define LEFT 'L'
#define RIGHT 'R'
#define STRAIGHT 'S'
#define HALT 'H'
void push_queue(int vertex);
int pop_queue();
int isEmpty_queue();
void initGraph();

struct Graph *createGraph(int V);
struct AdjListNode *newAdjListNode(int index, char * id);
struct GraphEdge *newGraphEdge(int start, int end, char dir);
int queue[MAX];
int front = -1;
int rear = -1;
int path_out[NUM_VERTICES];
char instructions[INSTRUCTIONS_SIZE];
struct Graph *graph;
struct GraphEdge *edges[NUM_EDGES];
int g_src;
int g_dest;

void push_queue(int vertex)
{
    if (rear == MAX - 1)
        printf("Queue Overflow\n");
    else
    {
        if (front == -1)
            front = 0;
        rear = rear + 1;
        queue[rear] = vertex;
    }
}

int isEmpty_queue()
{
    if (front == -1 || front > rear)
        return 1;
    else
        return 0;
}

int pop_queue()
{
    int delete_item;
    if (front == -1 || front > rear)
    {
        printf("Queue Underflow\n");
        exit(1);
    }

    delete_item = queue[front];
    front = front + 1;
    return delete_item;
}

// A structure to represent an adjacency list node
struct AdjListNode
{
    int index;
    char * id;
    struct AdjListNode *next;
};

struct GraphEdge
{
    int start;
    int end;
    char dir;
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
    struct AdjList *array;
};

#define RX_BUFFER_SIZE 80 // Software receive buffer size

static uint32_t rxDataReady = 0;               // Flag indicating receiver does not have data
static volatile char rxBuffer[RX_BUFFER_SIZE]; // Software receive buffer
static char txBuffer[RX_BUFFER_SIZE];          // Software transmit buffer

// A utility function to create a new adjacency list node
struct AdjListNode *newAdjListNode(int index, char * id)
{
    struct AdjListNode *newNode =
        (struct AdjListNode *)malloc(sizeof(struct AdjListNode));
    newNode->index = index;
    newNode->id = id;
    newNode->next = NULL;
    return newNode;
}

struct GraphEdge *newGraphEdge(int start, int end, char dir)
{
    struct GraphEdge *newEdge =
        (struct GraphEdge *)malloc(sizeof(struct GraphEdge));
    newEdge->start = start;
    newEdge->end = end;
    newEdge->dir = dir;
    return newEdge;
}

// A utility function that creates a graph of V vertices
struct Graph *createGraph(int V)
{
    struct Graph *graph =
        (struct Graph *)malloc(sizeof(struct Graph));
    graph->V = V;

    // Create an array of adjacency lists.  Size of
    // array will be V
    graph->array =
        (struct AdjList *)malloc(V * sizeof(struct AdjList));

    // Initialize each adjacency list as empty by
    // making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// Adds an edge to an undirected graph
void addEdge(struct Graph *graph, int src, int dest, char * srcId, char * destId, char srcType, char destType)
{
    // Add an edge from src to dest.  A new node is
    // added to the adjacency list of src.  The node
    // is added at the beginning
    struct AdjListNode *destNode = newAdjListNode(dest, destId);
    struct GraphEdge *destEdge = newGraphEdge(src, dest, srcType);

    for (int i = 0; i < NUM_EDGES; i++)
    {
        if (edges[i] == NULL)
        {
            edges[i] = destEdge;
            break;
        }
    }
    destNode->next = graph->array[src].head;
    graph->array[src].head = destNode;
    // Since graph is undirected, add an edge from
    // dest to src also
    struct AdjListNode *srcNode = newAdjListNode(src, srcId);
    struct GraphEdge *srcEdge = newGraphEdge(dest, src, destType);
    for (int i = 0; i < NUM_EDGES; i++)
    {
        if (edges[i] == NULL)
        {
            edges[i] = srcEdge;
            break;
        }
    }
    srcNode->next = graph->array[dest].head;
    graph->array[dest].head = srcNode;
}

// A utility function to print the adjacency list
// representation of graph
void printGraph(struct Graph *graph)
{
    int v;

    for (v = 0; v < graph->V; v++)
    {
        struct AdjListNode *pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n %d", v, v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->index);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

void printEdges()
{
    for (int v = 0; v < NUM_EDGES; v++)
    {
        if (edges[v] != NULL)
        {
            printf("edge %d to %d, %c\n", edges[v]->start, edges[v]->end, edges[v]->dir);
        }
    }
}

void bfs(struct Graph *graph, int V, int start, int end)
{
    memset(txBuffer, 0, 80);
    if (start < 0 || end < 0 || start > V - 1 || end > V - 1)
    {
        printf("Invalid start and end inputs.\n");
        return;
    }
    int v = start;
    int parent[V];
    int visited[V];
    int path[V];
    int length = 0;
    for (int j = 0; j < V; j++)
    {
        parent[j] = -1;
        path[j] = -1;
        visited[j] = 0;
    }
    // clear global path output variable
    for (int i = 0; i < NUM_VERTICES; i++)
    {
        path_out[i] = -1;
    }
    push_queue(v);

    while (!isEmpty_queue())
    {
        v = pop_queue();
        if (visited[v] == 1) // check if visited
            continue;
        visited[v] = 1; // if not, mark vertex as visited

        struct AdjListNode *s = graph->array[v].head; // get adjacency list of vertex

        // if end, backtrace and print path

        if (v == end)
        {
            int x = end;
            // build path by backtracing the parent
            for (int i = 0; i < V; i++)
            {
                if (x == start)
                    break;
                path[i] = x;
                x = parent[x];
            }
            for (int j = 0; j < V; j++)
            {
                if (path[j] != -1)
                {
                    length++;
                }
            }

            for (int i = 0; i < length + 1; i++)
            {
                if (i == 0)
                { // set first value to start
                    path_out[0] = start;
                    // printf("Path from vertex %d to %d: ", start, end);
                    // printf("%d -> ", start);
                    continue;
                }

                path_out[i] = path[length - i];
                // if(path_out[i] == end){
                //     printf("%d\n", path_out[i]);
                // }
                // else{
                //     printf("%d -> ", path_out[i]);
                // }
            }
            // clear the queue
            while (!isEmpty_queue())
            {
                pop_queue();
            }
            front = -1;
            rear = -1;

            break;
        }

        // else, go through adjacency list and push to queue

        while (s)
        {
            if (parent[s->index] == -1)
                parent[s->index] = v;
            push_queue(s->index);
            s = s->next;
        }
    }
}

// Driver program to test above functions
void initGraph()
{
    // CREATE GRAPH
    int V = NUM_VERTICES; // define number of vertices

    graph = createGraph(V);
    addEdge(graph, 0, 1, "0x8804F858", "0x8804BED0", STRAIGHT, STRAIGHT);
    addEdge(graph, 0, 3, "0x8804F858", "0x8804BE03", RIGHT, LEFT);
    addEdge(graph, 0, 2, "0x8804F858", "0x8804F058", LEFT, RIGHT);

    addEdge(graph, 1, 5, "0x8804BED0", "0x8804D0D3", LEFT, RIGHT);
    addEdge(graph, 1, 2, "0x8804BED0", "0x8804F058", RIGHT, LEFT);

    addEdge(graph, 2, 4, "0x8804F058", "0x8804E758", STRAIGHT, STRAIGHT);

    addEdge(graph, 3, 6, "0x8804BE03", "0x8804B757", RIGHT, LEFT);
    addEdge(graph, 3, 8, "0x8804BE03", "0x8804A657", STRAIGHT, STRAIGHT);

    addEdge(graph, 4, 6, "0x8804E758", "0x8804B757", LEFT, RIGHT);
    addEdge(graph, 4, 7, "0x8804E758", "0x8804A5D3", RIGHT, LEFT);
    addEdge(graph, 4, 9, "0x8804E758", "0x8804AE75", STRAIGHT, STRAIGHT);

    addEdge(graph, 5, 10, "0x8804D0D3", "0x8804DF58", STRAIGHT, STRAIGHT);
    addEdge(graph, 5, 7, "0x8804D0D3", "0x8804A5D3", LEFT, RIGHT);

    addEdge(graph, 6, 7, "0x8804B757", "0x8804A5D3", STRAIGHT, STRAIGHT);
    addEdge(graph, 6, 9, "0x8804B757", "0x8804AE57", LEFT, RIGHT);
    addEdge(graph, 6, 8, "0x8804B757", "0x8804A657", RIGHT, LEFT);

    addEdge(graph, 7, 10, "0x8804A5D3", "0x8804DF58", LEFT, RIGHT);
    addEdge(graph, 7, 9, "0x8804A5D3", "0x8804AE57", RIGHT, LEFT);

    addEdge(graph, 8, 11, "0x8804A657", "0x88048B57", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 9, 12, "0x8804AE57", "0x88049E57", STRAIGHT, STRAIGHT);

    addEdge(graph, 10, 20, "0x8804DF58", "0x88046556", RIGHT, LEFT);

    addEdge(graph, 11, 13, "0x88048B57", "0x88049357", RIGHT, LEFT);
    addEdge(graph, 11, 14, "0x88048B57", "0x88048357", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 12, 13, "0x88049E57","0x88049357", LEFT, RIGHT);
    addEdge(graph, 12, 15, "0x88049E57", "0x88047A57", STRAIGHT, STRAIGHT);

    addEdge(graph, 13, 14, "0x88049357", "0x88048357", RIGHT, LEFT);
    addEdge(graph, 13, 15, "0x88049357", "0x88047A57", LEFT, RIGHT);

    addEdge(graph, 14, 17, "0x88048357", "0x88046A58", RIGHT, LEFT);

    addEdge(graph, 15, 16, "0x88047A57", "0x88047257", STRAIGHT, STRAIGHT);

    addEdge(graph, 16, 17, "0x88047257", "0x88046A58", LEFT, RIGHT);
    addEdge(graph, 16, 18, "0x88047257", "0x8804C6D3", RIGHT, LEFT);

    addEdge(graph, 17, 18, "0x88046A58", "0x8804C6D3", STRAIGHT, STRAIGHT);

    addEdge(graph, 18, 19, "0x8804C6D3", "0x88044B56", STRAIGHT, STRAIGHT);

    addEdge(graph, 19, 22, "0x88044B56", "0x88045356", RIGHT, LEFT);
    addEdge(graph, 19, 23, "0x88044B56", "0x88044356", STRAIGHT, STRAIGHT);

    addEdge(graph, 20, 21, "0x88046556", "0x88045B56", LEFT, RIGHT);
    addEdge(graph, 20, 27, "0x88046556", "0x88046D56", STRAIGHT, STRAIGHT);

    addEdge(graph, 21, 27, "0x88045B56", "0x88046D56", LEFT, RIGHT);
    addEdge(graph, 21, 25, "0x88045B56", "0x88049DD3", RIGHT, LEFT);
    addEdge(graph, 21, 22, "0x88045B56", "0x88045356", STRAIGHT, STRAIGHT);

    addEdge(graph, 22, 25, "0x88045356", "0x88049DD3", LEFT, RIGHT);
    addEdge(graph, 22, 23, "0x88045356", "0x88044356", RIGHT, LEFT);

    addEdge(graph, 23, 24, "0x88044356", "0x880490D2", STRAIGHT, STRAIGHT);

    addEdge(graph, 24, 29, "0x880490D2", "0x88047857", RIGHT, LEFT);
    addEdge(graph, 24, 31, "0x880490D2", "0x880481D3", STRAIGHT, STRAIGHT);

    addEdge(graph, 25, 26, "0x88049DD3", "0x88047556", STRAIGHT, STRAIGHT);

    addEdge(graph, 26, 29, "0x88047556", "0x88047857", LEFT, RIGHT);
    addEdge(graph, 26, 30, "0x88047556", "0x8804B6D3", RIGHT, LEFT);

    addEdge(graph, 27, 28, "0x88046D56", "0x88047D56", STRAIGHT, STRAIGHT);

    addEdge(graph, 28, 30, "0x88047D56", "0x8804B6D3", LEFT, RIGHT);
    addEdge(graph, 28, 32, "0x88047D56", "0x88048157", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 29, 30, "0x88047857", "0x8804B6D3",  STRAIGHT, STRAIGHT);
    addEdge(graph, 29, 31, "0x88047857", "0x880481D3", RIGHT, LEFT);

    addEdge(graph, 30, 32, "0x8804B6D3", "0x88048157", LEFT, RIGHT);

    addEdge(graph, 31, 33, "0x880481D3", "0x8804C6D0", STRAIGHT, STRAIGHT);

    addEdge(graph, 32, 41, "0x88048157", "0x8804A6D0", RIGHT, LEFT);
    
    addEdge(graph, 33, 34, "0x8804C6D0", "0x880489D3", STRAIGHT, STRAIGHT);
    addEdge(graph, 33, 35, "0x8804C6D0", "0x8804D0D0", RIGHT, LEFT);

    addEdge(graph, 34, 37, "0x880489D3", "0x8804A3D4", RIGHT, LEFT);

    addEdge(graph, 35, 36, "0x8804D0D0", "0x880491D3", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 36, 38, "0x880491D3", "0x8804E7D0", LEFT, RIGHT);
    addEdge(graph, 36, 39, "0x880491D3", "0x8804A9D3", STRAIGHT, STRAIGHT); 

    addEdge(graph, 37, 38, "0x8804A3D4", "0x8804E7D0", RIGHT, LEFT);
    addEdge(graph, 37, 40, "0x8804A3D4", "0x8804A6D4", STRAIGHT, STRAIGHT);

    addEdge(graph, 38, 39, "0x8804E7D0", "0x8804A9D3", LEFT, RIGHT);
    addEdge(graph, 38, 40, "0x8804E7D0", "0x8804A6D4", RIGHT, LEFT);

    addEdge(graph, 40, 43, "0x8804A6D4", "0x8804C4D4", STRAIGHT, STRAIGHT);

    addEdge(graph, 41, 44, "0x8804A6D0", "0x8804ADD3", LEFT, RIGHT);
    addEdge(graph, 41, 46, "0x8804A6D0", "0x8804B4D4", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 42, 44, "0x8804BCD4", "0x8804ADD3", RIGHT, LEFT);
    addEdge(graph, 42, 45, "0x8804BCD4", "0x8804D8D0", LEFT, RIGHT);
    addEdge(graph, 42, 47, "0x8804BCD4", "0x8804E0D0",STRAIGHT, STRAIGHT);
    
    addEdge(graph, 43, 48, "0x8804C4D4", "0x8804CED4",STRAIGHT, STRAIGHT);
    addEdge(graph, 43, 45, "0x8804C4D4", "0x8804D8D0", LEFT, RIGHT); 

    addEdge(graph, 44, 45, "0x8804ADD3", "0x8804D8D0", STRAIGHT, STRAIGHT);
    addEdge(graph, 44, 46, "0x8804ADD3", "0x8804B4D4", LEFT, RIGHT);
    addEdge(graph, 44, 47, "0x8804ADD3", "0x8804E0D0", RIGHT, LEFT);
    
    addEdge(graph, 45, 47, "0x8804D8D0", "0x8804E0D0", LEFT, RIGHT);
    addEdge(graph, 45, 48, "0x8804D8D0", "0x8804CED4", RIGHT, LEFT);

    addEdge(graph, 46, 50, "0x8804B4D4", "0x8804D8D3", LEFT, RIGHT);
   
    addEdge(graph, 47, 49, "0x8804E0D0", "0x8804B6D0", STRAIGHT, STRAIGHT);
    
    addEdge(graph, 48, 51, "0x8804CED4",  "0x8804AED0", RIGHT, LEFT);

    addEdge(graph, 49, 50, "0x8804B6D0", "0x8804D8D3", RIGHT, LEFT);
    addEdge(graph, 49, 51, "0x8804B6D0", "0x8804AED0", LEFT, RIGHT);

    addEdge(graph, 50, 51, "0x8804D8D3", "0x8804AED0", STRAIGHT, STRAIGHT);


    g_src = START_VERTEX;
    g_dest = END_VERTEX;
}

void getEndVertex()
{
    if (txBuffer[0] == 'h')
    {
        g_dest = HOME;
    }
    else if (txBuffer[0] == 'w')
    {
        g_dest = WORK;
    }
    else if (txBuffer[0] == 's')
    {
        g_dest = SOLAR;
    }
    else if (txBuffer[0] == 'o')
    {
        g_dest = OUTAGE;
    }
}

void getStartVertex()
{
    g_src = 0;
}

char *getEdgeDir(int start, int end)
{
    for (int i = 0; i < NUM_EDGES; i++)
    {
        if (edges[i]->start == start && edges[i]->end == end)
        {
            return &edges[i]->dir;
        }
    }
    return "n";
}
void getPathOut()
{
    // edge[i,j]
    int i = 0;
    int j = 1;

    while (path_out[j] != -1)
    {
        char temp[20];
        sprintf(temp, "%s", graph->array[path_out[i]].head->id);
        strcat(instructions, temp);
        strcat(instructions, ",");
        strcat(instructions, getEdgeDir(path_out[i], path_out[j]));
        strcat(instructions, ",");
        i++;
        j++;
    }
}
void clearTxBuffer()
{
    for (int i = 0; i < RX_BUFFER_SIZE; i++)
    {
        txBuffer[i] = '\0'; // Copy rxBuffer into txBuffer
    }
}

void clearInstructions()
{
    for (int i = 0; i < INSTRUCTIONS_SIZE; i++)
    {
    instructions[i] = '\0';
    }
}
/**************************************************************************/ /**
                                                                              * @brief
                                                                              *    Main function
                                                                              *
                                                                              * @details
                                                                              *    Initialize the GPIO and LEUART modules. Send a welcome message over the TX
                                                                              *    pin. After the RX handler tells us that we have data, start
                                                                              *    processing the received data. Start by disabling RX and TX interrupts so
                                                                              *    that the data we received in rxBuffer does not get overwritten by one of
                                                                              *    the handlers before we read it. Copy the data in rxBuffer into the
                                                                              *    txBuffer and then trigger a TX interrupt to echo it out over the TX pin.
                                                                              *    If there is no data availabe, go into EM2 until an RX or TX interrupt
                                                                              *    wakes up the CPU.
                                                                              *****************************************************************************/
int main(void)
{

    // Initialization
    initGraph();
    //   printEdges();
    bfs(graph, NUM_VERTICES, 0, 5);
    
    getPathOut();

    for (int i = 0; i < INSTRUCTIONS_SIZE; i++)
    {
        printf("%c", instructions[i]);
    }

    printf("%s", "\n");

    clearInstructions();
    bfs(graph, NUM_VERTICES, 1, 9);
    getPathOut();
    for (int i = 0; i < INSTRUCTIONS_SIZE; i++)
    {
        printf("%c", instructions[i]);
    }
}
