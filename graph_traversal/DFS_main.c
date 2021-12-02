// DFS algorithm in C

#include <stdio.h>
#include <stdlib.h>

 
// Data structure to represent a stack
struct stack
{
    int maxsize;    // define max capacity of the stack
    int top;
    int *items;
};
 
// Utility function to initialize the stack
struct stack* newStack(int capacity)
{
    struct stack *pt = (struct stack*)malloc(sizeof(struct stack));
 
    pt->maxsize = capacity;
    pt->top = -1;
    pt->items = (int*)malloc(sizeof(int) * capacity);
 
    return pt;
}
 
// Utility function to return the size of the stack
int size(struct stack *pt) {
    return pt->top + 1;
}
 
// Utility function to check if the stack is empty or not
int isEmpty(struct stack *pt) {
    return pt->top == -1;                   // or return size(pt) == 0;
}
 
// Utility function to check if the stack is full or not
int isFull(struct stack *pt) {
    return pt->top == pt->maxsize - 1;      // or return size(pt) == pt->maxsize;
}
 
// Utility function to add an element `x` to the stack
void push(struct stack *pt, int x)
{
    // check if the stack is already full. Then inserting an element would
    // lead to stack overflow
    if (isFull(pt))
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }
 
    //printf("Inserting %d\n", x);
 
    // add an element and increment the top's index
    pt->items[++pt->top] = x;
}
 
// Utility function to return the top element of the stack
int peek(struct stack *pt)
{
    // check for an empty stack
    if (!isEmpty(pt)) {
        return pt->items[pt->top];
    }
    else {
        exit(EXIT_FAILURE);
    }
}
 
// Utility function to pop a top element from the stack
int pop(struct stack *pt)
{
    // check for stack underflow
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }
 
    //printf("Removing %d\n", peek(pt));
 
    // decrement stack size by 1 and (optionally) return the popped element
    return pt->items[pt->top--];
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct node {
  int vertex;
  struct node* next;
};

struct node* createNode(int v);

struct Graph {
  int numVertices;
  int* visited;

  // We need int** to store a two dimensional array.
  // Similary, we need struct node** to store an array of Linked lists
  struct node** adjLists;
};

int IsAdj(struct Graph* graph, int new, int val){ 
  struct node* temp = graph->adjLists[val];

  while(temp != NULL){
    if (temp->vertex == new){
      return 1;
    }
    temp = temp->next;
  }
  return 0;
}

// DFS algo (prints everything but the starting point)
int DFS(struct stack *ptr, struct stack *last_val, struct Graph* graph, int start, int end) {
  if (IsAdj(graph, end, start)){
    // this is my stop condition
    printf("Path found! \n");
    push(last_val, start);
    push(last_val, end);
    return 900;
  }

  graph->visited[start] = 1; 
  push(last_val, start);
  
  
  struct node* temp = graph->adjLists[start];

  while(temp != NULL){
    if (graph->visited[temp->vertex] == 0){
      push(ptr, temp->vertex);
    }
    
    temp = temp->next;
  }
  
  while(!IsAdj(graph, peek(ptr), peek(last_val))){
    pop(last_val);
  }

  DFS(ptr, last_val, graph, pop(ptr), end);
}

// Create a node
struct node* createNode(int v) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->vertex = v;
  newNode->next = NULL;
  return newNode;
}

// Create graph
struct Graph* createGraph(int vertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));
  graph->numVertices = vertices;

  graph->adjLists = malloc(vertices * sizeof(struct node*));

  graph->visited = malloc(vertices * sizeof(int));

  int i;
  for (i = 0; i < vertices; i++) {
    graph->adjLists[i] = NULL;
    graph->visited[i] = 0;
  }
  return graph;
}

// Add edge
void addEdge(struct Graph* graph, int src, int dest) {
  // Add edge from src to dest
  struct node* newNode = createNode(dest);
  newNode->next = graph->adjLists[src];
  graph->adjLists[src] = newNode;

  // Add edge from dest to src
  newNode = createNode(src);
  newNode->next = graph->adjLists[dest];
  graph->adjLists[dest] = newNode;
}

// Print the graph
void printGraph(struct Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n Adjacency list of vertex %d\n ", v);
    while (temp) {
      printf("%d -> ", temp->vertex);
      temp = temp->next;
    }
    printf("\n");
  }
}

//Reverse the path
void ReversePath(struct stack *reversed_path, char *path, int length){
  while(!isEmpty(reversed_path)){
    path[length] = pop(reversed_path);
    length--;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main() {
  struct Graph* graph = createGraph(19);
  struct stack *test1 = newStack(30);
  struct stack *reversed_path = newStack(30);
  

  addEdge(graph, 0, 1);
  addEdge(graph, 0, 9);
  addEdge(graph, 1, 2);
  addEdge(graph, 2, 3);
  addEdge(graph, 2, 7);
  addEdge(graph, 3, 4);
  addEdge(graph, 4, 5);
  addEdge(graph, 4, 14);
  addEdge(graph, 5, 6);
  addEdge(graph, 6, 13);
  addEdge(graph, 6, 7);
  addEdge(graph, 7, 8);
  addEdge(graph, 8, 11);
  addEdge(graph, 8, 9);
  addEdge(graph, 9, 10);
  addEdge(graph, 10, 11);
  addEdge(graph, 10, 18);
  addEdge(graph, 11, 12);
  addEdge(graph, 12, 16);
  addEdge(graph, 12, 13);
  addEdge(graph, 13, 14);
  addEdge(graph, 14, 15);
  addEdge(graph, 15, 16);
  addEdge(graph, 16, 17);
  addEdge(graph, 17, 18);
    
  DFS(test1, reversed_path, graph, 0, 15);

  int length = size(reversed_path);
  char path[length];

  ReversePath(reversed_path, path, length);

  for(int i=1; i<length; i++){
    printf("%d -> ", path[i]);
  }

  printf("%d", path[length]);
}