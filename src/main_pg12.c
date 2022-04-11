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
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "em_chip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usart.h"

#define MAX 10000
#define NUM_VERTICES 54 // 52
#define NUM_EDGES 178   // 174

#define START_VERTEX 0
#define END_VERTEX 9

#define HOME 0x8804B6D0
//Need to change these to the address of the locations
#define WORK 0x88046D56
#define OUTAGE HOME
#define SOLAR 0x8804F058

#define INSTRUCTIONS_SIZE 1000

#define LEFT "L"
#define RIGHT "R"
#define STRAIGHT "S"
#define HALT "H"
#define SLOWCHAR "Y"

void push_queue(uint32_t vertex);
int pop_queue();
int isEmpty_queue();
void initGraph();
void rm_first_letter(char * str, char * tmp);
void test_path(void);
void clearInstructions(void);

struct Graph *createGraph(int V);
struct AdjListNode *newAdjListNode(int index, uint32_t id);
struct GraphEdge *newGraphEdge(uint32_t start, uint32_t end, char * dir);
int get_id_index(uint32_t id);
int queue[MAX];
int front = -1;
int rear = -1;
int path_out[NUM_VERTICES];
char instructions[INSTRUCTIONS_SIZE];
struct Graph *graph;
struct GraphEdge *edges[NUM_EDGES];
uint32_t g_src;
uint32_t g_dest;
int previousID;

uint32_t tags[NUM_VERTICES];

void push_queue(uint32_t vertex)
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
  uint32_t index;
  uint32_t id;
  struct AdjListNode *next;
};

// A structure to represent an adjacency list
struct AdjList
{
  struct AdjListNode *head;
};

struct GraphEdge
{
  uint32_t start;
  uint32_t end;
  char * dir;
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

/**************************************************************************/ /**
* @brief
*    Initialize the GPIO pins for the LEUART module
*****************************************************************************/
void initGpio(void)
{
  // GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LEUART0 TX and RX pins
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);    // RX

  // Initialize USART2 TX and RX pins for BLE
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortA, 7, gpioModeInput, 0);    // RX

  // Initialize USART3 TX and RX pins for ARDUINO
  GPIO_PinModeSet(gpioPortB, 6, gpioModePushPull, 1); // USART3_TX
  GPIO_PinModeSet(gpioPortB, 7, gpioModeInput, 0);    // USART3_RX
}

/**************************************************************************/ /**
* @brief
*    Initialize the LEUART module
*****************************************************************************/
void initLeuart(void)
{
  // Enable LE (low energy) clocks
  CMU_ClockEnable(cmuClock_HFLE, true);             // Necessary for accessing LE modules
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO); // Set a reference clock

  // Enable clocks for LEUART0
  CMU_ClockEnable(cmuClock_LEUART0, true);
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1); // Don't prescale LEUART clock

  // Initialize the LEUART0 module
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  LEUART_Init(LEUART0, &init);

  // Enable LEUART0 RX/TX pins on PD[11:10] (see readme.txt for details)
  LEUART0->ROUTEPEN = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
  LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;

  // Enable LEUART0 RX/TX interrupts
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
  NVIC_EnableIRQ(LEUART0_IRQn);
}

/**************************************************************************/ /**
* @brief
*    LEUART0 interrupt service routine
*
* @details
*    Keep receiving data while there is still data left in the hardware RX buffer.
*    Store incoming data into rxBuffer and set rxDataReady when a linefeed '\n' is
*    sent or if there is no more room in the buffer.
*****************************************************************************/
void LEUART0_IRQHandler(void)
{
  // Note: These are static because the handler will exit/enter
  //       multiple times to fully transmit a message.
  static uint32_t rxIndex = 0;
  static uint32_t txIndex = 0;

  // Acknowledge the interrupt
  uint32_t flags = LEUART_IntGet(LEUART0);
  LEUART_IntClear(LEUART0, flags);

  // RX portion of the interrupt handler
  if (flags & LEUART_IF_RXDATAV)
  {
    while (LEUART0->STATUS & LEUART_STATUS_RXDATAV)
    { // While there is still incoming data
      char data = LEUART_Rx(LEUART0);
      if ((rxIndex < RX_BUFFER_SIZE - 2) && (data != '\n'))
      { // Save two spots for '\n' and '\0'
        rxBuffer[rxIndex++] = data;
      }
      else
      { // Done receiving
        rxBuffer[rxIndex++] = '\n';
        rxBuffer[rxIndex] = '\0';
        rxDataReady = 1;
        rxIndex = 0;
        break;
      }
    }
  }

  // TX portion of the interrupt handler
  if (flags & LEUART_IF_TXC)
  {
    if ((txIndex < RX_BUFFER_SIZE) && (txBuffer[txIndex] != '\0'))
    {
      LEUART_Tx(LEUART0, txBuffer[txIndex++]); // Send the data
    }
    else
    { // Done transmitting
      txIndex = 0;
    }
  }
}

// A utility function to create a new adjacency list node
struct AdjListNode *newAdjListNode(int index, uint32_t id)
{
    struct AdjListNode *newNode =
        (struct AdjListNode *)malloc(sizeof(struct AdjListNode));
    newNode->index = index;
    newNode->id = id;
    newNode->next = NULL;
    return newNode;
}

struct GraphEdge *newGraphEdge(uint32_t start, uint32_t end, char * dir)
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
void addEdge(struct Graph *graph, int src, int dest, char * srcType, char * destType)
{
    // Add an edge from src to dest.  A new node is
    // added to the adjacency list of src.  The node
    // is added at the beginning
    struct AdjListNode *destNode = newAdjListNode(dest, tags[dest]);
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
    struct AdjListNode *srcNode = newAdjListNode(src, tags[src]);
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
            printf("-> %ld", pCrawl->index);
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
      printf("edge %d to %d, %s\n", (int)edges[v]->start, (int)edges[v]->end, edges[v]->dir);
    }
  }
}

void bfs(struct Graph *graph, int V, int startID, int endID,  int prevID)
{
    memset(txBuffer, 0, 80);
    int start = get_id_index(startID);
    int end = get_id_index(endID);
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

    if(prevID != -2){
      visited[get_id_index(prevID)] = 1; //If prevID is not -1 (default meaning that the car has not went over a tag) then we set that ID as visited.
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

void initArray(void){
  tags[0] = 0x8804F858;
  tags[1] = 0x8804BED0;
  tags[2] = 0x8804F058;
  tags[3] = 0x8804BE03;
  tags[4] = 0x8804E758;
  tags[5] = 0x8804D0D3;
  tags[6] = 0x8804B757;
  tags[7] = 0x8804A5D3;
  tags[8] = 0x8804A657;
  tags[9] = 0x8804AE75;
  tags[10] = 0x8804DF58;
  tags[11] = 0x88048B57;
  tags[12] = 0x88049E57;
  tags[13] = 0x88049357;
  tags[14] = 0x88048357;
  tags[15] = 0x88047A57;
  tags[16] = 0x88047257;
  tags[17] = 0x88046A58;
  tags[18] = 0x8804C6D3;
  tags[19] = 0x88044B56;
  tags[20] = 0x88046556;
  tags[21] = 0x88045B56;
  tags[22] = 0x88045356;
  tags[23] = 0x88044356;
  tags[24] = 0x880490D2;
  tags[25] = 0x88049DD3;
  tags[26] = 0x88047556;
  tags[27] = 0x88046D56;
  tags[28] = 0x88047D56;
  tags[29] = 0x88047857;
  tags[30] = 0x8804B6D3;
  tags[31] = 0x880481D3;
  tags[32] = 0x88048157;
  tags[33] = 0x8804C6D0;
  tags[34] = 0x880489D3;
  tags[35] = 0x8804D0D0;
  tags[36] = 0x880491D3;
  tags[37] = 0x8804A3D4;
  tags[38] = 0x8804E7D0;
  tags[39] = 0x8804A9D3;
  tags[40] = 0x8804A6D4;
  tags[41] = 0x8804A6D0;
  tags[42] = 0x8804BCD4;
  tags[43] = 0x8804C4D4;
  tags[44] = 0x8804ADD3;
  tags[45] = 0x8804D8D0;
  tags[46] = 0x8804B4D4;
  tags[47] = 0x8804E0D0;
  tags[48] = 0x8804CED4;
  tags[49] = 0x8804B6D0;
  tags[50] = 0x8804D8D3;
  tags[51] = 0x8804AED0;
  // testing only
  tags[52] = 0x88049CD1;
  tags[53] = 0x8804B4D3;
}

int get_id_index(uint32_t id){
  for(int i = 0; i < NUM_VERTICES; i++){
      if(id == tags[i])
        return i;
  }
  return -1;
}

// Driver program to test above functions
void initGraph()
{
  // CREATE GRAPH
  int V = NUM_VERTICES; // define number of vertices

  graph = createGraph(V);
  addEdge(graph, 0, 1, STRAIGHT, STRAIGHT);
  addEdge(graph, 0, 3, RIGHT, LEFT);
  addEdge(graph, 0, 2, LEFT, RIGHT);

  addEdge(graph, 1, 5, LEFT, RIGHT);
  addEdge(graph, 1, 2, RIGHT, LEFT);

  addEdge(graph, 2, 4, STRAIGHT, STRAIGHT);

  addEdge(graph, 3, 6, RIGHT, LEFT);
  addEdge(graph, 3, 8, STRAIGHT, STRAIGHT);

  addEdge(graph, 4, 6, LEFT, RIGHT);
  addEdge(graph, 4, 7, RIGHT, LEFT);
  addEdge(graph, 4, 9, STRAIGHT, STRAIGHT);

  addEdge(graph, 5, 10, STRAIGHT, STRAIGHT);
  addEdge(graph, 5, 7, LEFT, RIGHT);

  addEdge(graph, 6, 7, STRAIGHT, STRAIGHT);
  addEdge(graph, 6, 9, LEFT, RIGHT);
  addEdge(graph, 6, 8, RIGHT, LEFT);

  addEdge(graph, 7, 10, LEFT, RIGHT);
  addEdge(graph, 7, 9, RIGHT, LEFT);

  addEdge(graph, 8, 11, STRAIGHT, STRAIGHT);

  addEdge(graph, 9, 12, STRAIGHT, STRAIGHT);

  addEdge(graph, 10, 20, RIGHT, LEFT);

  addEdge(graph, 11, 13, RIGHT, LEFT);
  addEdge(graph, 11, 14, STRAIGHT, STRAIGHT);

  addEdge(graph, 12, 13, LEFT, RIGHT);
  addEdge(graph, 12, 15, STRAIGHT, STRAIGHT);

  addEdge(graph, 13, 14, RIGHT, LEFT);
  addEdge(graph, 13, 15, LEFT, RIGHT);

  addEdge(graph, 14, 17, RIGHT, LEFT);

  addEdge(graph, 15, 16, STRAIGHT, STRAIGHT);

  addEdge(graph, 16, 17, LEFT, RIGHT);
  addEdge(graph, 16, 18, RIGHT, LEFT);

  addEdge(graph, 17, 18, STRAIGHT, STRAIGHT);

  addEdge(graph, 18, 19, STRAIGHT, STRAIGHT);

  addEdge(graph, 19, 22, RIGHT, LEFT);
  addEdge(graph, 19, 23, STRAIGHT, STRAIGHT);

  addEdge(graph, 20, 21, LEFT, RIGHT);
  addEdge(graph, 20, 27, STRAIGHT, STRAIGHT);

  addEdge(graph, 21, 27, LEFT, RIGHT);
  addEdge(graph, 21, 25, RIGHT, LEFT);
  addEdge(graph, 21, 22, STRAIGHT, STRAIGHT);

  addEdge(graph, 22, 25, LEFT, RIGHT);
  addEdge(graph, 22, 23, RIGHT, LEFT);

  addEdge(graph, 23, 24, STRAIGHT, STRAIGHT);

  addEdge(graph, 24, 29, RIGHT, LEFT);
  addEdge(graph, 24, 31, STRAIGHT, STRAIGHT);

  addEdge(graph, 25, 26, STRAIGHT, STRAIGHT);

  addEdge(graph, 26, 29, LEFT, RIGHT);
  addEdge(graph, 26, 30, RIGHT, LEFT);

  addEdge(graph, 27, 28, STRAIGHT, STRAIGHT);

  addEdge(graph, 28, 30, LEFT, RIGHT);
  addEdge(graph, 28, 32, STRAIGHT, STRAIGHT);

  addEdge(graph, 29, 30, STRAIGHT, STRAIGHT);
  addEdge(graph, 29, 31, RIGHT, LEFT);

  addEdge(graph, 30, 32, LEFT, RIGHT);

  addEdge(graph, 31, 33, STRAIGHT, STRAIGHT);

  addEdge(graph, 32, 41, RIGHT, LEFT);

  addEdge(graph, 33, 34, STRAIGHT, STRAIGHT);
  addEdge(graph, 33, 35, RIGHT, LEFT);

  addEdge(graph, 34, 37, RIGHT, LEFT);

  addEdge(graph, 35, 36, STRAIGHT, STRAIGHT);

  addEdge(graph, 36, 38, LEFT, RIGHT);
  addEdge(graph, 36, 39, STRAIGHT, STRAIGHT);

  addEdge(graph, 37, 38, RIGHT, LEFT);
  addEdge(graph, 37, 40, STRAIGHT, STRAIGHT);

  addEdge(graph, 38, 39, LEFT, RIGHT);
  addEdge(graph, 38, 40, RIGHT, LEFT);

  addEdge(graph, 40, 43, STRAIGHT, STRAIGHT);

  addEdge(graph, 41, 44, LEFT, RIGHT);
  addEdge(graph, 41, 46, STRAIGHT, STRAIGHT);

  addEdge(graph, 42, 44, RIGHT, LEFT);
  addEdge(graph, 42, 45, LEFT, RIGHT);
  addEdge(graph, 42, 47, STRAIGHT, STRAIGHT);

  addEdge(graph, 43, 48, STRAIGHT, STRAIGHT);
  addEdge(graph, 43, 45, LEFT, RIGHT);

  addEdge(graph, 44, 45, STRAIGHT, STRAIGHT);
  addEdge(graph, 44, 46, LEFT, RIGHT);
  addEdge(graph, 44, 47, RIGHT, LEFT);

  addEdge(graph, 45, 47, LEFT, RIGHT);
  addEdge(graph, 45, 48, RIGHT, LEFT);

  addEdge(graph, 46, 50, LEFT, RIGHT);

  addEdge(graph, 47, 49, STRAIGHT, STRAIGHT);

  addEdge(graph, 48, 51, RIGHT, LEFT);

  addEdge(graph, 49, 50, RIGHT, LEFT);
  addEdge(graph, 49, 51, LEFT, RIGHT);

  addEdge(graph, 50, 51, STRAIGHT, STRAIGHT);

  // testing only
  addEdge(graph, 52, 53, LEFT, RIGHT);
  addEdge(graph, 53, 49, STRAIGHT, STRAIGHT);

  g_src = START_VERTEX;
  g_dest = END_VERTEX;
}

void getEndVertex()
{
  if (rxBuffer[0] == 'h')
  {
    g_dest = HOME;
  }
  else if (rxBuffer[0] == 'w')
  {
    g_dest = WORK;
  }
  else if (rxBuffer[0] == 's')
  {
    g_dest = SOLAR;
  }
  else if (rxBuffer[0] == 'o')
  {
    g_dest = OUTAGE;
  }
}

void getStartVertex()
{
  g_src = 0;
}

char * getEdgeDir(int start, int end)
{
  for (int i = 0; i < NUM_EDGES; i++)
  {
    if (edges[i]->start == start && edges[i]->end == end)
    {
      return edges[i]->dir;
    }
  }
  return "n";
}

void getPathOut()
{
  // edge[i,j]
  clearInstructions();
  int i = 0;

  char temp[20];
  char end[20];

  strcat(instructions, "#");
  while (path_out[i+2] != -1)
  {
//      sprintf(temp, "%d", (int)graph->array[path_out[i]].head->id);
      sprintf(temp, "%d", (int)tags[path_out[i]]);
      strcat(instructions, temp);
      strcat(instructions, ",");
      strcat(instructions, getEdgeDir(path_out[i], path_out[i+1]));
      strcat(instructions, ",");
      i++;
  }

  if(path_out[i+1] != -1){
    sprintf(temp, "%d", (int)tags[path_out[i]]);
    strcat(instructions, temp);
    strcat(instructions, ",");
    // Put the slow character in front of the second to last command (example = "SR" for slow right)
    // only time this doesn't happen is when path is single instruction
    strcat(instructions, SLOWCHAR);
    strcat(instructions, getEdgeDir(path_out[i], path_out[i+1]));
    strcat(instructions, ",");
  }

  sprintf(end, "%d", (int)g_dest);
  strcat(instructions, end);
  strcat(instructions, ",");
  strcat(instructions, HALT);
  strcat(instructions, "!");
}

void clearTxBuffer()
{
  for (int i = 0; i < RX_BUFFER_SIZE; i++)
  {
    txBuffer[i] = '\0'; // Copy rxBuffer into txBuffer
  }
}

void writeToPWA(char * data)
{
  LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Disable interrupts

  uint32_t i;
  for (i = 0; data[i] != 0; i++)
  {
    txBuffer[i] = data[i]; // Copy data into txBuffer
  }

  txBuffer[i] = '\0';
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Re-enable interrupts
  LEUART_IntSet(LEUART0, LEUART_IFS_TXC);
}

void rm_first_letter(char * str, char * tmp){
  for(int i = 0; i < strlen(str); i++){
      tmp[i] = str[i + 1];
  }
}

void clearInstructions(void)
{
    for (int i = 0; i < INSTRUCTIONS_SIZE; i++)
    {
    instructions[i] = '\0';
    }
}
/**************************************************************************/
/**
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

int main(void){
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  initLeuart();
  initGraph();
  initArray();
  ble_usart_open();

  g_dest = HOME;
  previousID = -2;

  test_path();

  char str[40];
  char tmp[40];

  g_dest = HOME;

  // int battery;
  uint32_t id;

  LEUART_IntSet(LEUART0, LEUART_IFS_TXC);


  while (1)
  {

    // When notified by the RX handler, start processing the received data
    if (rxDataReady) // received from PWA
    {
      LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Disable interrupts


//--------------------Comms between model and PWA ---------------------------------------
      // DIVIDER only do this depending on case
      clearInstructions();

//      getStartVertex(); // set source
      getEndVertex();   // set destination

      bfs(graph, NUM_VERTICES, g_src, g_dest, previousID);
      getPathOut();

      ble_write(instructions); // send to car
      //DIVIDER end
//-----------------------------------------------------------------------------------------------------------

      rxDataReady = 0;  // Indicate that we need new data

      LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Re-enable interrupts
      LEUART_IntSet(LEUART0, LEUART_IFS_TXC);
    }

    // Test reads with usart BLE

    // This will be handling communication coming from Car
    if (ble_newData())   // received from car
    {
      ble_read(str); // uses strcpy, assumes communication is chars/strings



//--------------------Comms between model and car ---------------------------------------
      // get battery
      if(str[0] == 'b' && strlen(str) > 2){
          rm_first_letter(str, tmp);
          //battery = atoi(tmp);
          writeToPWA(str);
      }

      // check if lost
      if(str[0] == 'r' && strlen(str) > 2){
          rm_first_letter(str, tmp);
          id = atoi(tmp);


          // check that the id is valid
          if((id & (0x8804 << 16)) == (0x8804 << 16)){
              // recalculate based on id
              if(get_id_index(id) != -1){
               // if id is valid recalculate

                if(previousID == -2){
                   previousID = id;
                }
               else{
                   previousID = g_src;
               }
               g_src = id;
               bfs(graph, NUM_VERTICES, g_src, g_dest, previousID);
               getPathOut();
               ble_write(instructions); // send to car
              }
          }
       }

      if(str[0] == 'I' && strlen(str) > 2){
          rm_first_letter(str, tmp);
          id = atoi(tmp);
          if((id & (0x8804 << 16)) == (0x8804 << 16)){
              // if id is valid, update previousID
              if(get_id_index(id) != -1){
                if(previousID == -2){
                     previousID = id;
                  }
                  else{
                     previousID = g_src;
                  }
              }
              g_src = id;

          }
      }

      // check low battery
      if(str[0] == 'L' && str[1] == 'B'){
          // set destination to charger
          // car clears its path and will then go through lost protocol
//          g_dest = SOLAR;   // disabled for testing
      }

      // Check if arrived
      if(str[0] == 'A' && str[1] == 'V'){
          //Send the PWA character A to change screen and reactivate buttons so that the user can choose what they want to do at location
          writeToPWA("a");
        //   g_src = g_dest;
      }

      // Timeout
      if(str[0] == 'T' && str[1] == 'O'){
          //This means that the car has not hit an NFC tag in 4 seconds
          //Car turns off motors and flashes the error LED
          writeToPWA("e"); //Send the error command to the PWA
//          g_dest = HOME; //Set the g_dest to home so that we see something similar to the initialization state
      }

      //Car has finished charging
      if(str[0] == 'S' && str[1] == 'C'){
          //This means that the car has finished charging itself
          writeToPWA("d"); //Send command to tell PWA that charging is done and buttons can be reactivated
          //Car should then be sent a new path based on the input from the user
      }
    }
//-----------------------------------------------------------------------------------------------------------


    // lowest energy mode we can enter is EM1 due to USART
    EMU_EnterEM1();
  }
}


void test_path(void){
  g_dest = HOME;
  printGraph(graph);
  bfs(graph, NUM_VERTICES, 0x88049CD1, g_dest, previousID);
  getPathOut();
  ble_write(instructions); // send to car
  clearInstructions();

//  g_dest = 0x8804A6D0;
  bfs(graph, NUM_VERTICES, 0x8804B4D3, g_dest, previousID);
  getPathOut();
  ble_write(instructions); // send to car
  clearInstructions();

  bfs(graph, NUM_VERTICES, 0x88049E57, g_dest, previousID);
  getPathOut();
  ble_write(instructions); // send to car
  clearInstructions();
}

