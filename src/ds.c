#include "ds.h"
#include "string.h"
#include "io.h"

// Linked List Definitions

#define ELEMENT_SIZE 128
#define EMPTY -1


/*
 / Linked List Code
*/

char buf[4096];

// memory management variables
struct listElement *elementPool = (struct listElement *)buf;
struct listElement *listHead = NULL;
int elementInUse[ELEMENT_SIZE]; // keep track of element usage (max 128 elements)

// initialize memory pool and usage tracker
void listInit() {
	for (int i = 0; i < ELEMENT_SIZE; i++) elementInUse[i] = 0; // all elements are initially set to zero
}

// function to allocate unused element from buf
struct listElement *allocateElement() {
	for (int i = 0; i < ELEMENT_SIZE; i++) {
		if (!elementInUse[i]) {
			elementInUse[i] = 1; // mark element as used
			return &elementPool[i];
		}
	}
	return NULL;
}

// function to release an element back to the pool
void listFreeElement(struct listElement *element) {
	for (int i = 0; i < ELEMENT_SIZE; i++) {
		if (&elementPool[i] == element) {
			elementInUse[i] = 0; // mark element as free
			return; 
		}
	}
}

// function to add a new element to the tail of the list
void listAdd(struct listElement **listHead, struct listElement *newElement) {
	if (newElement == NULL) return; // confirm that the new element is NOT null
	newElement->next = NULL; // set the new element's next pointer to null

	// if the list is empty, make the new element the head
	if (*listHead == NULL) *listHead = newElement;
	else { // traverse the list to find the last element
		struct listElement *curr = *listHead;
		while (curr->next != NULL) curr = curr->next;
		curr->next = newElement;
	}
}

void listRemove(struct listElement *element) {
	if (element == NULL || listHead == NULL) return; // if list is empty, return
	
	// removing head of the list
	if (listHead == element) {
		listHead = element->next;
		listFreeElement(element);
		return;
	}

	// traversing the list to find element's predecessor
	struct listElement *curr = listHead;
	while (curr->next != NULL && curr->next != element) curr = curr->next;

	// if element is found, unlink it
	if (curr ->next == element) {
		curr->next = element->next;
		listFreeElement(element);
	}
}

// function to print list to serial port
void listPrint() {
	struct listElement *curr = listHead;
	while (curr != NULL) {
		printp("%d -> ", curr->data);
		curr = curr->next;
	}
	printp("NULL\n");
}


/*
 / Queue Code
*/

void queueInit(struct Queue *queue) {
	queue->front = 0;
	queue->rear = -1;
	queue->size = 0;
}

int queueIsEmpty(struct Queue *queue) {
	if (queue->size == 0) return 1;
	return 0;
}

int queueIsFull(struct Queue *queue) {
	if (queue->size == QUEUE_CAP) return 1;
	return 0;
}

void enqueue(struct Queue *queue, int val) {
	if (queueIsFull(queue) == 1) {
		printp("Queue is full.\n");
		return;
	}
	queue->rear = (queue->rear + 1) % QUEUE_CAP;
	queue->data[queue->rear] = val;
	queue->size++;
	return;
}

int dequeue(struct Queue *queue) {
	if (queueIsEmpty(queue) == 1) return -1;
	int val = queue->data[queue->front];
	queue->front = (queue->front + 1) % QUEUE_CAP;
	queue->size--;
	return val;
}

int queuePeek(struct Queue *queue) {
	if (queueIsEmpty(queue) == 1) {
		printp("Queue is empty.\n");
		return -1;
	}
	return queue->data[queue->front];
}

void queuePrint(struct Queue *queue) {
	if (queueIsEmpty(queue) == 1) {
		printp("Queue is empty.\n");
		return;
	}
	for (int i = 0; i < queue->size; i++) {
		int idx = (queue->front + i) % QUEUE_CAP;
		printp("%d ", queue->data[idx]);
	}
	printp("\n");
}

/*
 / Stack Code
*/

void stackInit(Stack *stack) {
	stack->top = -1;
}

int stackIsFull(Stack *stack) {
	if (stack->top == STACK_SIZE - 1) return 1;
	return 0;
}

int stackIsEmpty(Stack *stack) {
	if (stack->top == -1) return 1;
	return 0;
}

void stackPush(Stack *stack, int val) {
	if (stackIsFull(stack) == 1) {
		printp("Stack Overflow(tm).\n");
		return;
	}
	stack->data[++stack->top] = val;
	return;
}

int stackPop(Stack *stack) {
	if (stackIsEmpty(stack) == 1) {
		printp("Stack is empty.\n");
		return STACK_EMPTY_VAL;
	}
	return stack->data[stack->top--];
}

int stackPeek(Stack *stack) {
	if (stackIsEmpty(stack) == 1) {
		printp("Stack is empty.\n");
		return STACK_EMPTY_VAL;
	}
	return stack->data[stack->top];
}

void stackPrint(Stack *stack) {
	if (stackIsEmpty(stack) == 1) {
		printp("Stack is empty.\n");
		return;
	}
	printp("%d", stack->data[0]);

	for (int i = 1; i <= stack->top; i++) printp(", %d", stack->data[i]);
	
	printp("\n");
	return;
}

/*
 / HashSet Code
*/

void setInit(HashSet *set) {
	for (int i = 0; i < HASHSET_SIZE; i++) set->data[i] = EMPTY;
}

unsigned int setHash(int key) {
	return (unsigned int)(key % HASHSET_SIZE);
}

void setAdd(HashSet *set, int key) {
	if (setContains(set, key) == 1 || setSize(set) == HASHSET_SIZE) {
		printp("Cannot add element.\n");
		return;
	}
	unsigned int hash = setHash(key);
	for (int i = 0; i < HASHSET_SIZE; i++) {
		unsigned int idx = (hash + i) % HASHSET_SIZE;
		if (set->data[idx] == EMPTY || set->data[idx] == key) {
			set->data[idx] = key;
			return;
		}
	}
	return;
}	

int setRemove(HashSet *set, int key) {
	unsigned int hash = setHash(key);
	for (int i = 0; i < HASHSET_SIZE; i++) {
		unsigned int idx = (hash + i) % HASHSET_SIZE;
		if (set->data[idx] == EMPTY) break;
		if (set->data[idx] == key) {
			set->data[idx] = EMPTY;
			return 1;
		}
	}
	return 0;
}

int setContains(HashSet *set, int key) {
	unsigned int hash = setHash(key);
	for (int i = 0; i < HASHSET_SIZE; i++) {
		unsigned int idx = (hash + i) % HASHSET_SIZE;
		if (set->data[idx] == EMPTY) break;
		if (set->data[idx] == key) return 1;
	}
	return 0;
}

int setSize(HashSet *set) {
	int count = 0;
	for (int i = 0; i < HASHSET_SIZE; i++) {
		if (set->data[i] != EMPTY) count++;
	}
	return count;
}

void setPrint(HashSet *set) {
	int isFirst = 1;
	printp("{");
	for (int i = 0; i < HASHSET_SIZE; i++) {
		if (set->data[i] != EMPTY) {
			if (!isFirst) printp(", ");
			printp("%d", set->data[i]);
			isFirst = 0;
		}
	}
	printp("}\n");
}

/*
 / HashMap Code
*/

void hashmapInit(HashMap *map) {
	
	// set all table entries to null
	for (int i = 0; i < TABLE_SIZE; i++) map->table[i] = NULL;
	
	// Allocate memory for each node and set up the free list
	for (int i = 0; i < MAX_NODES; i++) {
		map->nodes[i].key[0] = '\0';
		map->nodes[i].val = 0;
		map->nodes[i].next = (i < MAX_NODES - 1) ? &map->nodes[i + 1] : NULL;
	}	

	// set the free list of nodes to the address of nodes[0]
	map->freeList = &map->nodes[0];

}

unsigned int hash(const char *key) {
	unsigned int hash = 0;
	while (*key) {
		hash = (hash * 31) + *key;
		key++;
	}
	return hash % TABLE_SIZE;
}

HashMapNode *hashmapAllocateNode(HashMap *map, const char *key, int val) {

	// no available nodes
	if (map->freeList == NULL) {
		printp("Free list is empty\n");
		return NULL;
	}

	// update the freeList
	HashMapNode *node = map->freeList;
	map->freeList = map->freeList->next;

	strCopy(node->key, key, sizeof(node->key) - 1);
	node->key[sizeof(node->key) - 1] = '\0'; // null termination
	node->val = val;
	node->next = NULL;

	return node;
}

void hashmapPut(HashMap *map, const char *key, int val) {

	unsigned int index = hash(key);
	HashMapNode *newNode = hashmapAllocateNode(map, key, val);

	if (newNode == NULL) {
		printp("Out of free nodes\n");
		return; // failed to allocate
	}
	
	if (map->table[index] == NULL) map->table[index] = newNode;
	else {
		HashMapNode *curr = map->table[index];
		HashMapNode *prev = NULL;
		
		// traverse to the end of the chain or find an existing key
		while (curr != NULL) {
			if (strEqual(curr->key, key)) {
				
				// key exists, update value
				curr->val = val; 

				// return unused newNode to free list
				newNode->next = map->freeList;
				map->freeList = newNode;
				return;
			}
			prev = curr;
			curr = curr->next;
		}

		// key does not exist, append the new node at the end
		prev->next = newNode;
	}
}

int hashmapContainsKey(const HashMap *map, const char *key) {
	for (int i = 0; i < TABLE_SIZE; i++) {
		HashMapNode *curr = map->table[i];
		while (curr != NULL) {
			if (strEqual(curr->key, key)) return 1;
			curr = curr->next;
		}
	}
	return 0;
}

int hashmapGet(HashMap *map, const char *key) {
	unsigned int index = hash(key);
	HashMapNode *curr = map->table[index];

	while (curr != NULL) {
		if (strEqual(curr->key, key)) return curr->val;
		curr = curr->next;
	}
	return 0;
}

void hashmapFreeNode(HashMap *map, HashMapNode *node) {
	if (node != NULL) {
		node->next = map->freeList;
		map->freeList = node;
	}
}

void hashmapFreeListPrint(const HashMap *map) {
	printp("Free list:\n");
	HashMapNode *curr = map->freeList;
	while (curr != NULL) {
		printp("HashMapNode key: %s\n", curr->key[0] ? curr->key : "(empty)");
		curr = curr->next;
	}
}

void hashmapPrint(const HashMap *map) {
	printp("{");
	int isFirst = 1; // track the first entry
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (map->table[i] != NULL) {
			HashMapNode *curr = map->table[i];
				while (curr != NULL) {

					// if it's not the first entry, place comma followed by k/v
					if (!isFirst) printp(", ");

					printp("%s=%d", curr->key, curr->val);
					isFirst = 0; // set flag for all subsequent entries
					curr = curr->next;
			}
		}
	}
	printp("}\n");
}

/*
 / Heap Code
*/

void heapInit(Heap *heap) {
	heap->size = 0;	
}

// MaxHeap Functions

int maxHeapExtract(Heap *heap) {
	if (heap->size <= 0) {
		printp("Heap is empty!\n");
		return -1;
	}
	// grab max value from index 0 of heap
	int max = heap->data[0];

	// reduce size of heap and reflect new index 0 value
	heap->data[0] = heap->data[--heap->size];

	// heapify down with new max as the index
	maxHeapifyDown(heap, 0);
	return max;
}

int maxHeapInsert(Heap *heap, int val) {
	if (heap->size >= HEAP_SIZE) {
		printp("Heap is full.\n");
		return 0;
	}
	// set the element at the array of heap size to the input value
	heap->data[heap->size] = val;

	// heapify up with new element as the index
	maxHeapifyUp(heap, heap->size);

	// increment the size value of the heap
	heap->size++;
	return 1;
}

int maxHeapDelete(Heap *heap, int val) {
	int idx = -1;
	for (int i = 0; i < heap->size; i++) {
		if (heap->data[i] == val) {
			idx = i;
			break;
		}
	}
	if (idx == -1) {
		printp("Value not found.\n");
		return -1;
	}

	// replace value with the last element
	heap->data[idx] = heap->data[--heap->size];

	// restore heap property
	int parent = (idx - 1) / 2;

	// if index if greater than 0 and heap at index is greater than heap at parent...
	if (idx > 0 && heap->data[idx] > heap->data[parent]) maxHeapifyUp(heap, idx);
	else maxHeapifyDown(heap, idx);
	return 1;
}

void maxHeapifyUp(Heap *heap, int idx) {
	int parent = (idx - 1) / 2;
	while (idx > 0 && heap->data[idx] > heap->data[parent]) {
		int tmp = heap->data[idx];
		heap->data[idx] = heap->data[parent];
		heap->data[parent] = tmp;

		idx = parent;
		parent = (idx - 1) / 2;
	}
}

void maxHeapifyDown(Heap *heap, int idx) {
	int max = idx;
	int l = 2 * idx + 1, r = 2 * idx + 2;

	if (l < heap->size && heap->data[l] > heap->data[max]) max = l;
	if (r < heap->size && heap->data[r] > heap->data[max]) max = r;
	if (max != idx) {
		int tmp = heap->data[idx];
		heap->data[idx] = heap->data[max];
		heap->data[max] = tmp;

		maxHeapifyDown(heap, max);
	}
}

int maxHeapValidate(Heap *heap) {
	for (int i = 0; i < heap->size / 2; i++) {
		int l = 2 * i + 1, r = 2 * i + 2;
		if ((l < heap->size && heap->data[i] < heap->data[l]) || (r < heap->size && heap->data[i] < heap->data[r])) return 0;
	}
	return 1;
}



// MinHeap Functions

int minHeapExtract(Heap *heap) {
	if (heap->size <= 0) {
		printp("Heap is empty!\n");
		return -1;
	}
	// grab min value from index 0 of heap
	int min = heap->data[0];

	// reduce size of heap and reflect new index 0 value
	heap->data[0] = heap->data[--heap->size];

	// heapify down with new min as the index
	minHeapifyDown(heap, 0);
	return min;
}
// not finished
int minHeapInsert(Heap *heap, int val) {
	if (heap->size >= HEAP_SIZE) {
		printp("Heap is full.\n");
		return 0;
	}
	// set the element at the array of heap size to the input value
	heap->data[heap->size] = val;

	// heapify up with new element as the index
	minHeapifyUp(heap, heap->size);

	// increment the size value of the heap
	heap->size++;
	return 1;
}

int minHeapDelete(Heap *heap, int val) {
	int idx = -1;
	for (int i = 0; i < heap->size; i++) {
		if (heap->data[i] == val) {
			idx = i;
			break;
		}
	}
	if (idx == -1) {
		printp("Value not found.\n");
		return -1;
	}

	// replace value with the last element
	heap->data[idx] = heap->data[--heap->size];

	// restore heap property
	int parent = (idx - 1) / 2;

	// if index if greater than 0 and heap at index is greater than heap at parent...
	if (idx > 0 && heap->data[idx] < heap->data[parent]) minHeapifyUp(heap, idx);
	else minHeapifyDown(heap, idx);
	return 1;
}


// not finished
void minHeapifyUp(Heap *heap, int idx) {
	int parent = (idx - 1) / 2;
	while (idx > 0 && heap->data[idx] < heap->data[parent]) {
		int tmp = heap->data[idx];
		heap->data[idx] = heap->data[parent];
		heap->data[parent] = tmp;

		idx = parent;
		parent = (idx - 1) / 2;
	}
}

void minHeapifyDown(Heap *heap, int idx) {
	int min = idx;
	int l = 2 * idx + 1, r = 2 * idx + 2;

	if (l < heap->size && heap->data[l] < heap->data[min]) min = l;
	if (r < heap->size && heap->data[r] < heap->data[min]) min = r;
	if (min != idx) {
		int tmp = heap->data[idx];
		heap->data[idx] = heap->data[min];
		heap->data[min] = tmp;

		minHeapifyDown(heap, min);
	}
}


// not finished
int minHeapValidate(Heap *heap) {
	for (int i = 0; i < heap->size / 2; i++) {
		int l = 2 * i + 1, r = 2 * i + 2;
		if ((l < heap->size && heap->data[i] < heap->data[l]) || (r < heap->size && heap->data[i] < heap->data[r])) return 0;
	}
	return 1;
}


// Heap Conversion Functions (min->max, max->min)

void minHeapToMaxHeap(Heap *heap) {
	for (int i = (heap->size / 2) - 1; i >= 0; i--) maxHeapifyDown(heap, i);
}

void maxHeapToMinHeap(Heap *heap) {
	for (int i = (heap->size / 2) - 1; i >= 0; i--) minHeapifyDown(heap, i);
}


// Heap Print

void heapPrint(Heap *heap) {
	if (heap->size == 0) {
		printp("Heap is empty.\n");
		return;
	}
	
	int level = 0, nodesPrinted = 0, nodesInLevel = 1;
	
	for (int i = 0; i < heap->size; i++) {
		printp("%d ", heap->data[i]);
		nodesPrinted++;

		if (nodesPrinted == nodesInLevel) {
			printp("\n");
			level++;
			nodesPrinted = 0;
			nodesInLevel = 1 << level;
		}
	}
	printp("\n");
}

/*
 * Vector Code
 */

void vectorInit(Vector *vec){
        vec->size = 0;
        vec->data[0] = 0;
}

int vectorIsFull(Vector *vec) {
        if (vec->size == VEC_CAP) {
                printp("Vector is full.\n");
                return 1;
        }
        return 0;
}

int vectorIsEmpty(Vector *vec) {
        if (vec->size == 0) return 1;
        return 0;
}

int vectorSize(Vector *vec) { return vec->size; }

int vectorPushBack(Vector *vec, int val) {
        if (vectorIsFull(vec)) return -1;
        if (vectorIsEmpty(vec)) {
                vec->data[0] = val;
                vec->size = 1;
                return 1;
        }
        vec->data[vec->size] = val;
        vec->size++;
        return 1;
}

int vectorPopBack(Vector *vec) {
        if (vectorIsEmpty(vec)) return -1;
        int res = vec->data[vec->size - 1];
        vec->size--;
        return res;
}

int vectorBack(Vector *vec) {
        if (vectorIsEmpty(vec)) return -1;
        return vec->data[vec->size - 1];
}

void vectorPrint(Vector *vec) {
        if (vectorIsEmpty(vec)) {
                printp("Vector is empty.\n");
                return;
        }
        printp("[%d", vec->data[0]);
        for (int i = 1; i < vec->size; i++) printp(", %d", vec->data[i]);
        printp("]\n");
        return;
}
