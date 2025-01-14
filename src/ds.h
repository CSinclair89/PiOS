#ifndef DS_H
#define DS_H

/*
 * The Data Structures are implemented in the following order:
 * 1.) Definitions
 * 	a. Linked List
 * 	b. Queue
 * 	c. Stack
 * 	d. HashSet
 * 	e. HashMap
 * 	f. Heap
 * 	g. Vector
 * 2.) Structs & Declarations
 * 	a. Linked List
 * 	b. Queue
 * 	c. Stack
 * 	d. HashSet
 * 	e. HashMap
 * 	f. Heap
 * 	g. Vector
 * 3.) Function Headers
 * 	a. Linked List
 * 	b. Queue
 * 	c. Stack
 * 	d. HashSet
 * 	e. HashMap
 * 	f. Heap
 * 	g. Vector
 *
 * END
 */

// 1.a) Linked List definitions
#define BUFFER_SIZE 4096
#define ELEMENT_SIZE 128
#define NULL (void *)0

// 1.b) Queue definitions
#define QUEUE_CAP 128

// 1.c) Stack definitions
#define STACK_SIZE 100
#define STACK_EMPTY_VAL -1

// 1.d) HashSet definitions
#define HASHSET_SIZE 100

// 1.e) HashMap definitions
#define TABLE_SIZE 10
#define MAX_NODES 10

// 1.f) Heap definitions
#define HEAP_SIZE 100

// 1.g) Vector definitions
#define VEC_CAP 5


// 2.a) Linked List structs & declarations
struct listElement {
	int data;
	struct listElement *next;
};

extern struct listElement *listHead;
extern char buf[BUFFER_SIZE];
extern struct listElement *elementPool;
extern int elementInUse[ELEMENT_SIZE];

// 2.b) Queue structs & declarations
struct Queue {
	int data[QUEUE_CAP];
	int front;
	int rear;
	int size;
};

// 2.c) Stack structs & declarations
typedef struct Stack {
	int data[STACK_SIZE];
	int top;
} Stack;

// 2.d) HashSet structs & declarations
typedef struct HashSet {
	int data[HASHSET_SIZE];
} HashSet;

// 2.e) HashMap structs & declarations
typedef struct HashMapNode {
        char key[32];
        int val;
        struct HashMapNode *next;
} HashMapNode;

typedef struct HashMap {
        HashMapNode *table[TABLE_SIZE];
        HashMapNode nodes[MAX_NODES];
        HashMapNode *freeList;
} HashMap;

// 2.f) Heap structs & declarations
typedef struct Heap {
	int data[HEAP_SIZE];
	int size;
} Heap;

// 2.g) Vector structs & declarations
typedef struct Vector {
        int size;
        int data[VEC_CAP];
} Vector;


// 3.a) Linked List function definitions
void listInit();
struct listElement *allocateElement(void);

void listFreeElement(struct listElement *element);
void listAdd(struct listElement **listHead, struct listElement *newElement);
void listRemove(struct listElement *element);
void listPrint();

// 3.b)  Queue function definitions
void queueInit(struct Queue *queue);
int queueIsFull(struct Queue *queue);
int queueIsEmpty(struct Queue *queue);
void enqueue(struct Queue *queue, int val);
int dequeue(struct Queue *queue);
int queuePeek(struct Queue *queue);
void queuePrint(struct Queue *queue);

// 3.c) Stack function definitions
void stackInit(Stack *stack);
int stackIsFull(Stack *stack);
int stackIsEmpty(Stack *stack);
void stackPush(Stack *stack, int val);
int stackPop(Stack *stack);
int stackPeek(Stack *stack);
void stackPrint(Stack *stack);

// 3.d) HashSet function definitions
void setInit(HashSet *set);
unsigned int setHash(int key);
void setAdd(HashSet *set, int key);
int setRemove(HashSet *set, int key);
int setContains(HashSet *set, int key);
int setSize(HashSet *set);
void setPrint(HashSet *set);

// 3.e) HashMap function definitions
void hashmapInit(HashMap *map);
unsigned int hash(const char *key);
HashMapNode *hashmapAllocateNode(HashMap *map, const char *key, int val);
void hashmapPut(HashMap *map, const char *key, int val);
int hashmapGet(HashMap *map, const char *key);
int hashmapContainsKey(const HashMap *map, const char *key);
void hashmapFreeNode(HashMap *map, HashMapNode *node);
void hashmapFreeListPrint(const HashMap *map);
void hashmapPrint(const HashMap *map);

// 3.f) Heap function definitions
void heapInit(Heap *heap);

// MaxHeap Functions
int maxHeapExtract(Heap *heap);
int maxHeapInsert(Heap *heap, int val);
int maxHeapDelete(Heap *heap, int val);
void maxHeapifyUp(Heap *heap, int idx);
void maxHeapifyDown(Heap *heap, int idx);
int maxHeapValidate(Heap *heap);

// MinHeap Functions
int minHeapExtract(Heap *heap);
int minHeapInsert(Heap *heap, int val);
int minHeapDelete(Heap *heap, int val);
void minHeapifyUp(Heap *heap, int idx);
void minHeapifyDown(Heap *heap, int idx);
int minHeapValidate(Heap *heap);

// Heap Conversion Functions (min->max, max->min)
void minHeapToMaxHeap(Heap *heap);
void maxHeapToMinHeap(Heap *heap);

// Heap Print
void heapPrint(Heap *heap);

// 3.g) Vector Functions
void vectorInit(Vector *vec);
int vectorIsFull(Vector *vec);
int vectorIsEmpty(Vector *vec);
int vectorSize(Vector *vec);
int vectorPushBack(Vector *vec, int val);
int vectorPopBack(Vector *vec);
int vectorBack(Vector *vec);
void vectorPrint(Vector *vec);

#endif
