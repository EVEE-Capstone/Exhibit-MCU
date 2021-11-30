// A C Program to demonstrate adjacency list 
// representation of graphs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100  

int queue[MAX];
int front = -1;
int rear = -1;
void push_queue(int vertex);
int pop_queue();
int isEmpty_queue();

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
  
int * bfs(struct Graph* graph, int V, int start, int end){
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
                
            int path_out[length + 1];

            for(int i = 0; i < length + 1; i++){
                if(i == 0){ //set first value to path length
                    path_out[0] = length;
                    // printf("Path Length: %d\n", path_out[0]);
                    printf("Path from vertex %d to %d: ", start, end);
                    printf("%d -> ", start);
                    continue;
                }
                
                path_out[i] = path[length - i];
                if(path_out[i] == end){
                    printf("%d\n", path_out[i]);
                }
                else{
                    printf("%d -> ", path_out[i]);
                }
            }
            //clear the queue
            while(!isEmpty_queue()){
                pop_queue();
            }
            front = -1;
            rear = -1;

            return path_out;
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
    return NULL;
}

// Driver program to test above functions
int main()
{
    // CREATE GRAPH
    int V = 19; //define number of vertices
    
    struct Graph* graph = createGraph(V);
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

    // print the adjacency list representation of the above graph
    // printGraph(graph);

    int * p1 = bfs(graph, V, 0, 13); // expected: 0 -> 9 -> 10 -> 18
    int * p2 = bfs(graph, V, 1, 5); // expected: 1 -> 2 -> 7 -> 6 -> 5
    int * p3 = bfs(graph, V, 2, 13); // expected: 2 -> 7 -> 6 -> 13
    int * p4 = bfs(graph, V, 13, 14); // expected: 13 -> 14
    int * p5 = bfs(graph, V, 17, 5); // expected: 17 -> 16 -> 15 -> 14 -> 4 -> 5

    return 0;
}