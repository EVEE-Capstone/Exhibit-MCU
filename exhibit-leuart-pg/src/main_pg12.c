/***************************************************************************//**
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

#define MAX 100
#define NUM_VERTICES  19

#define START_VERTEX 0
#define END_VERTEX 9
#define HOME 1
#define WORK 12
#define SUPER 5
#define SOLAR 17

void push_queue(int vertex);
int pop_queue();
int isEmpty_queue();
void initGraph();

struct Graph* createGraph(int V);
struct AdjListNode* newAdjListNode(int val);
int queue[MAX];
int front = -1;
int rear = -1;
int path_out[NUM_VERTICES];
struct Graph* graph;
int g_src;
int g_dest;

void push_queue(int vertex)
{
   if(rear == MAX-1)
      printf("Queue Overflow\n");
   else
   {
      if(front == -1)
         front = 0;
      rear = rear+1;
      queue[rear] = vertex ;
   }
}

int isEmpty_queue()
{
   if(front == -1 || front > rear)
      return 1;
   else
      return 0;
}

int pop_queue()
{
   int delete_item;
   if(front == -1 || front > rear)
   {
      printf("Queue Underflow\n");
      exit(1);
   }

   delete_item = queue[front];
   front = front+1;
   return delete_item;
}

// A structure to represent an adjacency list node
struct AdjListNode
{
    int val;
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


#define RX_BUFFER_SIZE 80             // Software receive buffer size

static uint32_t rxDataReady = 0;      // Flag indicating receiver does not have data
static volatile char rxBuffer[RX_BUFFER_SIZE]; // Software receive buffer
static char txBuffer[RX_BUFFER_SIZE]; // Software transmit buffer

/**************************************************************************//**
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

/**************************************************************************//**
 * @brief
 *    Initialize the LEUART module
 *****************************************************************************/
void initLeuart(void)
{
  // Enable LE (low energy) clocks
  CMU_ClockEnable(cmuClock_HFLE, true); // Necessary for accessing LE modules
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO); // Set a reference clock

  // Enable clocks for LEUART0
  CMU_ClockEnable(cmuClock_LEUART0, true);
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1); // Don't prescale LEUART clock

  // Initialize the LEUART0 module
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  LEUART_Init(LEUART0, &init);

  // Enable LEUART0 RX/TX pins on PD[11:10] (see readme.txt for details)
  LEUART0->ROUTEPEN  = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
  LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;

  // Enable LEUART0 RX/TX interrupts
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
  NVIC_EnableIRQ(LEUART0_IRQn);
}

/**************************************************************************//**
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
  if (flags & LEUART_IF_RXDATAV) {
    while (LEUART0->STATUS & LEUART_STATUS_RXDATAV) { // While there is still incoming data
      char data = LEUART_Rx(LEUART0);
      if ((rxIndex < RX_BUFFER_SIZE - 2) && (data != '\n')) { // Save two spots for '\n' and '\0'
        rxBuffer[rxIndex++] = data;
      } else { // Done receiving
        rxBuffer[rxIndex++] = '\n';
        rxBuffer[rxIndex] = '\0';
        rxDataReady = 1;
        rxIndex = 0;
        break;
      }
    }
  }

  // TX portion of the interrupt handler
  if (flags & LEUART_IF_TXC) {
    if ((txIndex < RX_BUFFER_SIZE) && (txBuffer[txIndex] != '\0')) {
      LEUART_Tx(LEUART0, txBuffer[txIndex++]); // Send the data
    } else { // Done transmitting
      txIndex = 0;
    }
  }
}


// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int val)
{
    struct AdjListNode* newNode =
     (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->val = val;
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

    for (v = 0; v < graph->V; v++)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n %d (%s) ", v, v, pCrawl->type);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->val);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

void bfs(struct Graph* graph, int V, int start, int end){
    memset(txBuffer, 0, 80);
    if(start < 0 || end < 0 || start > V - 1 || end > V - 1){
        printf("Invalid start and end inputs.\n");
        return;
    }
    int v = start;
    int parent[V];
    int visited[V];
    int path[V];
    int length = 0;
    for(int j = 0; j < V; j++){
        parent[j] = -1;
        path[j] = -1;
        visited[j] = 0;
    }
    //clear global path output variable
    for(int i = 0; i < NUM_VERTICES; i++){
        path_out[i] = -1;
    }
    push_queue(v);

    while(!isEmpty_queue()){
        v = pop_queue();
        if(visited[v] == 1) //check if visited
            continue;
        visited[v] = 1; //if not, mark vertex as visited

        struct AdjListNode* s = graph->array[v].head; //get adjacency list of vertex

        //if end, backtrace and print path

        if(v == end){
            int x = end;
            //build path by backtracing the parent
            for(int i = 0; i < V; i++){
                if(x == start) break;
                path[i] = x;
                x = parent[x];
            }
            for(int j = 0; j < V; j++){
                if(path[j] != -1){
                    length++;
                }
            }

            for(int i = 0; i < length + 1; i++){
                if(i == 0){ //set first value to start
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
            //clear the queue
            while(!isEmpty_queue()){
                pop_queue();
            }
            front = -1;
            rear = -1;

            break;
        }

        //else, go through adjacency list and push to queue

        while(s){
            if(parent[s->val] == -1)
                parent[s->val] = v;
            push_queue(s->val);
            s = s->next;
        }


    }
}

// Driver program to test above functions
void initGraph()
{
    // CREATE GRAPH
    int V = NUM_VERTICES; //define number of vertices

    graph = createGraph(V);
    addEdge(graph, 0, 1, "road", "dest");
    addEdge(graph, 1, 2, "dest", "road");
    addEdge(graph, 2, 3, "road", "road");
    addEdge(graph, 0, 9, "road", "road");
    addEdge(graph, 2, 7, "road", "road");
    addEdge(graph, 3, 4, "road", "road");
    addEdge(graph, 4, 5, "road", "dest");
    addEdge(graph, 5, 6, "dest", "road");
    addEdge(graph, 6, 7, "road", "road");
    addEdge(graph, 7, 8, "road", "road");
    addEdge(graph, 8, 9, "road", "road");
    addEdge(graph, 4, 14, "road", "road");
    addEdge(graph, 6, 13, "road", "road");
    addEdge(graph, 8, 11, "road", "road");
    addEdge(graph, 9, 10, "road", "road");
    addEdge(graph, 10, 11, "road", "road");
    addEdge(graph, 11, 12, "road", "road");
    addEdge(graph, 12, 13, "road", "road");
    addEdge(graph, 13, 14, "road", "road");
    addEdge(graph, 14, 15, "road", "road");
    addEdge(graph, 10, 18, "road", "road");
    addEdge(graph, 12, 16, "road", "road");
    addEdge(graph, 15, 16, "road", "road");
    addEdge(graph, 16, 17, "road", "dest");
    addEdge(graph, 18, 17, "road", "dest");

    g_src = START_VERTEX;
    g_dest = END_VERTEX;

}

void getEndVertex()
{
  if(txBuffer[0] == 'h'){
     g_dest = HOME;
  }
  else if(txBuffer[0] == 'w'){
      g_dest = WORK;
  }
  else if(txBuffer[0] == 'c'){
      g_dest = SUPER;
  }
  else if(txBuffer[0] == 's'){
      g_dest = SOLAR;
  }
}

void getStartVertex()
{
  g_src = 0;
}
/**************************************************************************//**
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
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  initLeuart();
  initGraph();
  ble_usart_open();

  char str[20];

  // TODO: remove after test
  ble_write("#test!");     // print back what was read test only

  LEUART_IntSet(LEUART0, LEUART_IFS_TXC);

  uint32_t i;
  while (1) {

    // When notified by the RX handler, start processing the received data
    if (rxDataReady) {
      LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Disable interrupts

      for (i = 0; rxBuffer[i] != 0; i++) {
        txBuffer[i] = rxBuffer[i]; // Copy rxBuffer into txBuffer
      }

      getStartVertex();
      getEndVertex();

      bfs(graph, NUM_VERTICES, g_src, g_dest);

      for(i = 0; i < NUM_VERTICES; i++){
          sprintf(txBuffer, "%s%d,", txBuffer, path_out[i]);
      }

      printf("%s", txBuffer);
      txBuffer[i - 1] = '\0';

      rxDataReady = 0; // Indicate that we need new data
      LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC); // Re-enable interrupts
      LEUART_IntSet(LEUART0, LEUART_IFS_TXC);
    }

    // Test reads with usart BLE
    if(ble_newData()){      // ble_newData serves same purpose as rxDataReady above
        ble_read(str);      // uses strcpy, assumes communication is chars/strings

        ble_write(str);     // print back what was read
        ble_write("\n");

    }

    // lowest energy mode we can enter is EM1 due to USART
    EMU_EnterEM1();
  }
}

