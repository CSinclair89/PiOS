#include "ds.h"
#include "mmu.h"
#include "string.h"
#include "algos.h"
#include "tree.h"
#include "page.h"
#include "mem.h"
#include "io.h"

// mapping

void mmuTests() {
	
	printp("--MMU TEST--\n\n");

	// initialize the page frame allocator
	init_pfa_list();
	freeList = &physPageArray[0];

	struct ppage *testPage1 = allocatePhysPages(1);
	if (testPage1) printp("Allocated success (0x%x)\n", testPage1);
	else printp("Allocate failed.\n");

	struct ppage *testPage2 = allocatePhysPages(1);
	if (testPage2) printp("Allocated success (0x%x)\n", testPage2);
	else printp("Allocate failed.\n");

	freePhysPages(testPage2);
	freePhysPages(testPage1);
/*
	// define a virtual address
	void *vaddr = (void *)0x40000000;

	// allocate the physical page
	struct ppage *allocatedPage = allocatePhysPages(1);
	if (allocatedPage == NULL) printp("Cannot allocate page\n");
	void *paddr = allocatedPage->physAddr;

	mapPages(vaddr, paddr);
	printp("Mapped virtual address 0x%x to physical address 0x%x!\n", vaddr, paddr);

	void *vaddr2 = (void *)0x80000000;
	void *paddr2 = allocateAndMapPage(vaddr2);
	
	printp("Mapped virtual address (0x%x) to physical address (0x%x)\n", vaddr2, paddr2);

	printp("\n");

	printp("Test msg (virtual).\n");
	printp("Test msg (physical).\n");
	checkMMUState();

	void *vaddrSerial = (void *)0xC0000040;
	void *paddrSerial = (void *)0x3F215040;
	mapPagesIO(vaddrSerial, paddrSerial);
	mmu_on();
	__asm__ volatile("dsb ish");
	__asm__ volatile("isb");
	checkMMUState();
	printp("Mapped physical serial address (0x%x) to virtual address (0x%x).\n", paddrSerial, vaddrSerial);
	
	setupSerialMapping();
	printp("Test msg (physical).\n");
	printp("Test msg (virtual).\n");

	printp("Testing virtual-physical address translation outside of serial\n");
	void *vaddrTest = (void *)0xC0000000;
	void *paddrTest = (void *)0x400000;
	mapPages(vaddrTest, paddrTest);
	mmu_on();	
	
	// Use Barriers to Ensure Changes Take Effect
	__asm__ volatile("dsb sy");
	__asm__ volatile("isb");
	
	volatile unsigned int *testAddr = (volatile unsigned int *)vaddrTest;
	*testAddr = 0xDEADBEEF;
	printp("Value at vaddr 0x%x: 0x%x\n", vaddrTest, *testAddr);

	
	char testChar = 'A';
	int res = putc(testChar);
	int serialTest = checkSerialPortMapping();
	if (serialTest == 1) printp("Serial port is correctly mapped!\n");
	else printp("Serial port NOT correctly mapped.\n");
	
	if (res == testChar) printp("Test passsed!\n");
	else printp("Test failed\n");

	

	int *testPtr = (int *)vaddr;
	*testPtr = 42;
	printp("Value (%d) written to virtual address (0x%x)\n", testPtr, *testPtr);
*/
	printp("\n");

	printp("MMU Test 2 begins.\n");

	// need 3 inputs, virtual address, allocated page, and page directory
	void *vaddr3 = (void *)0x400000; // aligned to 2MB
	struct ppage *newPage = allocatePhysPages(1);
	struct page_directory pd = {0};

	if (newPage) {
	        void *mappedAddr = mapPages(vaddr3, newPage, &pd);
		if (mappedAddr) {
			printp("Mapped vaddr (0x%x) to paddr (0x%x).\n", mappedAddr, newPage->physAddr);
		} else {
			printp("Failed to map.\n");
		}
	}

	printp("Initial mapping was successful.\n");
	freePhysPages(newPage);

	printp("\n");

	printp("Testing serial port mapping:\n");

	printp("\n");
	
	/* serial port address mapping
	void *vaddrSerial = (void *)0x40000000;
	void *paddrSerial = (void *)0x3F215040;

	void *serialAddr = mapAddress(vaddrSerial, paddrSerial);
	if (serialAddr) printp("Printing with putv. Vaddr: 0x%x | Paddr: 0x%x\n", vaddrSerial, paddrSerial);
	else printp("Printing with putp because I suck at this...\n");
*/

	printp("Testing\n\n");


	printp("\n");

	printp("Testing mapping of one page:\n");
	struct ppage *onePage = allocatePhysPages(1);
	void *oneVaddr = (void *)0xC0000000;
	struct page_directory onePd = {0};

	if (mapPages(oneVaddr, onePage, &onePd)) {
		printp("Mapped vaddr (0x%x) to paddr (0x%x)\n", oneVaddr, onePage->physAddr);

		int *testInt = (int *)oneVaddr;
		*testInt = 42;
		int vaddrVal = *(int *)oneVaddr;
		int paddrVal = *(int *)mapAddress(oneVaddr, onePage->physAddr);

		printp("Value at vaddr (0x%x) = %d\n", oneVaddr, vaddrVal);
		printp("Value at paddr (0x%x) = %d\n", onePage->physAddr, paddrVal);


	} else {
		printp("Mapping failed\n");
	}


	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");
}

void listTests() {
	
	printp("--LINKED LISTED TEST--\n\n");

	// initialize list
	listInit();

	// add elements
	struct listElement *a = allocateElement();
        struct listElement *b = allocateElement();
        struct listElement *c = allocateElement();
        struct listElement *d = allocateElement();
        
        // define data for list elements
        a->data = 10;
        b->data = 20;
        c->data = 30;
        d->data = 40;

        // add list elements to list using listHead address as starting point
        listAdd(&listHead, a);
        listAdd(&listHead, b);
        listAdd(&listHead, c);
        listAdd(&listHead, d);

        printp("Initial Linked List: \n");
        listPrint();

	printp("\n");

        listRemove(b);
        printp("Linked List after removing 20: \n");
        listPrint();

	printp("\n");

        listRemove(a);
        listRemove(c);
        listRemove(d);
        printp("Linked List after removing 10, 30, and 40: \n"); 
        listPrint();

	printp("\n");
	printp("Linked List test complete.\n");

}

void mapTests() {

	printp("--HASHMAP TEST--\n\n");

	// initialize map
	HashMap map;
	hashmapInit(&map);

	// add entries to map
	hashmapPut(&map, "apples", 6);
	hashmapPut(&map, "beans", 52);
	hashmapPut(&map, "potatoes", 2);

	printp("Initial HashMap: \n");
	hashmapPrint(&map);

	printp("\n");

	// update an existing entry
	hashmapPut(&map, "apples", 13);
	printp("HashMap after updating apples value: \n");
	hashmapPrint(&map);

	printp("\n");

	// add a new entry to check chaining
	hashmapPut(&map, "bananas", 1);
	printp("HashMap after adding a new entry (to test chaining): \n");
	hashmapPrint(&map);

	printp("\n");

	// manually free a node
	unsigned int index = hash("beans");
	HashMapNode *nodeToFree = map.table[index];

	if (nodeToFree != NULL && strEqual(nodeToFree->key, "beans")) {

		// remove node from table
		map.table[index] = nodeToFree->next;
		hashmapFreeNode(&map, nodeToFree); // add removed node to free list
	}

	printp("HashMap after freeing node w/ key 'beans':\n");
	hashmapPrint(&map);

	printp("\n");

	// get the value of present key
	printp("Get the value of key 'potatoes':\n");
	printp("%d\n", hashmapGet(&map, "potatoes"));

	printp("\n");

	// get value of non-present key
	printp("Get the value of non-present key 'twix' (should return 0):\n");
	printp("%d\n", hashmapGet(&map, "twix"));

	printp("\n");
	printp("HashMap test complete.\n");

}

void queueTests() {

	printp("--QUEUE TEST--\n\n");

	struct Queue queue;
	queueInit(&queue);

	enqueue(&queue, 10);
	enqueue(&queue, 20);
	enqueue(&queue, 30);

	printp("Queue elements: ");
	queuePrint(&queue);

	printp("\n");

	printp("Dequeued: %d\n", dequeue(&queue));

	printp("\n");

	printp("Queue after dequeue: ");
	queuePrint(&queue);

	printp("\n");

	printp("Front element: %d\n", queuePeek(&queue));

	printp("\n");
	printp("Queue test complete.\n");

}


void stackTests() {

	printp("--STACK TEST--\n\n");	
	
	Stack stack;
	stackInit(&stack);
	stackPush(&stack, 6);
	stackPush(&stack, 81);
	stackPush(&stack, 4);
	stackPush(&stack, 14);

	printp("Initial stack (bottom to top):\n");	
	stackPrint(&stack);

	printp("\n");	

	printp("Peeking at top element (should be 14):\n");
	printp("%d\n", stackPeek(&stack));	

	printp("\n");	

	printp("Popping top two elements (should be 14, 4):\n");
	printp("%d, %d\n", stackPop(&stack), stackPop(&stack));

	printp("\n");	

	printp("Printing stack after above elements are popped:\n");
	stackPrint(&stack);

	printp("\n");	

	printp("Adding one more element to top of stack:\n");
	stackPush(&stack, 7);
	stackPrint(&stack);

	printp("\n");	

	printp("Stack after popping remaining elements: \n");
	stackPop(&stack);
	stackPop(&stack);
	stackPop(&stack);

	stackPrint(&stack);	

	printp("\n");	

	printp("Stack test complete.\n");	
}

void setTests() {

	printp("--SET TEST--\n\n");

	HashSet set;
	setInit(&set);

	printp("Set size without any elements: ");	

	printp("%d\n", setSize(&set));	

	printp("\n");	
	
	setAdd(&set, 17);
	setAdd(&set, 8);
	setAdd(&set, 91);
	printp("Set with 3 elements added:\n");
	setPrint(&set);	

	printp("\n");

	setAdd(&set, 22);
	printp("Adding another element:\n");
	setPrint(&set);	

	printp("\n");	

	printp("Confirming that set contains 91 (should return 1):\n");
	printp("%d\n", setContains(&set, 91));	

	printp("\n");	

	printp("Confirming that set does NOT contain 54 (should return 0):\n");
	printp("%d\n", setContains(&set, 54));	

	printp("\n");	

	printp("Set size:\n");
	printp("%d\n", setSize(&set));	

	printp("\n");	

	setRemove(&set, 8);
	setRemove(&set, 91);
	printp("Removing 2 elements and printing new set:\n");	
	setPrint(&set);

	printp("\n");	

	printp("Trying to add an element already in the set (22):\n");
	setAdd(&set, 22);

	printp("\n");
	printp("Set test complete.\n");
}

void heapTests() {
	printp("--HEAP TEST--\n\n");

	Heap heap;
	heapInit(&heap);

	maxHeapInsert(&heap, 8);
	maxHeapInsert(&heap, 12);
	maxHeapInsert(&heap, 21);
	maxHeapInsert(&heap, 3);
	maxHeapInsert(&heap, 89);
	maxHeapInsert(&heap, 23);
	maxHeapInsert(&heap, 45);
	maxHeapInsert(&heap, 66);
	maxHeapInsert(&heap, 17);
	maxHeapInsert(&heap, 7);
	maxHeapInsert(&heap, 30);

	printp("Testing Max Heap Functions:\n");
	heapPrint(&heap);

	printp("\n");

	printp("Removing root element of max heap: %d\n", maxHeapExtract(&heap));
	heapPrint(&heap);

	printp("\n");

	printp("Removing non-max element from max heap: %d\n", 45);
	maxHeapDelete(&heap, 45);
	heapPrint(&heap);

	printp("\n");

	printp("Adding one more element to max heap: %d\n", 19);
	maxHeapInsert(&heap, 19);
	heapPrint(&heap);

	printp("\n");

	printp("Turning max heap into min heap:\n");
	maxHeapToMinHeap(&heap);
	heapPrint(&heap);

	printp("\n");

	printp("Removing all elements from min heap:\n");
	minHeapDelete(&heap, 3);
	minHeapDelete(&heap, 7);
	minHeapDelete(&heap, 8);
	minHeapDelete(&heap, 12);
	minHeapDelete(&heap, 17);
	minHeapDelete(&heap, 19);
	minHeapDelete(&heap, 21);
	minHeapDelete(&heap, 23);
	minHeapDelete(&heap, 30);
	minHeapDelete(&heap, 66);

	heapPrint(&heap);

	printp("\n");

	printp("Testing Min Heap Functions:\n");
	minHeapInsert(&heap, 4);
	minHeapInsert(&heap, 11);
	minHeapInsert(&heap, 22);
	minHeapInsert(&heap, 40);
	minHeapInsert(&heap, 41);
	minHeapInsert(&heap, 57);
	minHeapInsert(&heap, 60);
	minHeapInsert(&heap, 78);
	minHeapInsert(&heap, 91);
	minHeapInsert(&heap, 13);
	heapPrint(&heap);	
	
	printp("\n");
	
	printp("Removing root element of min heap: %d\n", minHeapExtract(&heap));
	heapPrint(&heap);	

	printp("\n");
	
	printp("Removing non-min element from min heap: %d\n", 41);
	minHeapDelete(&heap, 41);
	heapPrint(&heap);
	
	printp("\n");

	printp("Adding one more element to the min heap: %d\n", 19);
	minHeapInsert(&heap, 19);
	heapPrint(&heap);

	printp("\n");

	printp("Turning min heap into max heap:\n");
	minHeapToMaxHeap(&heap);
	heapPrint(&heap);

	printp("\n");

	printp("Removing all elements from max heap:\n");

	maxHeapDelete(&heap, 11);
	maxHeapDelete(&heap, 22);
	maxHeapDelete(&heap, 40);
	maxHeapDelete(&heap, 57);
	maxHeapDelete(&heap, 60);
	maxHeapDelete(&heap, 78);
	maxHeapDelete(&heap, 91);
	maxHeapDelete(&heap, 19);
	maxHeapDelete(&heap, 13);
	heapPrint(&heap);

	printp("\n");

	printp("Heap test complete.\n");
}

void treeTests() {

	printp("--TREE TEST--\n\n");

	printp("\n");

	printp("Initial tree:\n");
	TreeNode tree;
	treeInsertNode(&tree, 42);
	treeInsertNode(&tree, 2);
	treeInsertNode(&tree, 83);
	treeInsertNode(&tree, 13);
	treeInsertNode(&tree, 23);
	treeInsertNode(&tree, 8);
	treeInsertNode(&tree, 40);
	treeInsertNode(&tree, 104);
	treeInsertNode(&tree, 36);
	treeInsertNode(&tree, 20);
	treePrintInOrder(&tree);

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");

	printp("\n");
}

void vectorTests() {

	printp("--VECTOR TEST--\n");

	printp("\n");

	printp("Initializing vector:\n");

	Vector vec;
	vectorInit(&vec);
	vectorPrint(&vec);
	
	printp("\n");
	
	printp("Adding element 7:\n");
	vectorPushBack(&vec, 7);
	vectorPrint(&vec);
	
	printp("\n");
	
	printp("Adding second element, 8:\n");
	vectorPushBack(&vec, 8);
	vectorPrint(&vec);

	printp("\n");

	printp("Adding 3 more elements, 81, 566, 3:\n");
	vectorPushBack(&vec, 81);
	vectorPushBack(&vec, 566);
	vectorPushBack(&vec, 3);
	vectorPrint(&vec);

	printp("\n");

	printp("Peeking at last element from the vector (without removing): %d\n", vectorBack(&vec));
	vectorPrint(&vec);

	printp("\n");

	printp("Popping the last element from the vector: %d\n", vectorPopBack(&vec));
	vectorPrint(&vec);

	printp("\n");

	printp("Testing cap limit of 5 elements (adding two elements):\n");
	vectorPushBack(&vec, 90);
	vectorPushBack(&vec, 78);

	printp("\n");

	printp("Printing vector to confirm all 5 elements after msg:\n");
	vectorPrint(&vec);

	printp("\n");

	printp("Removing 3 elements from vector:\n");
	vectorPopBack(&vec);
	vectorPopBack(&vec);
	vectorPopBack(&vec);
	vectorPrint(&vec);

	printp("\n");

	printp("Checking vector size: %d\n", vectorSize(&vec));

	printp("\n");

	printp("Removing remaining 2 elements and printing:\n");
	vectorPopBack(&vec);
	vectorPopBack(&vec);
	vectorPrint(&vec);

	printp("\n");
}


/*
 * Algorithm Tests
 */

void arraySortTests() {

	int nums[10] = {8, 91, 67, 34, 52, 1, 50, 66, 17, 64};


}

void duplicateNumTest() {
	printp("--DUPLICATE NUMBER TEST--\n\n");

	int numsTrue[10] = {6, 2, 88, 45, 98, 52, 88, 41, 7, 19};
	int numSize1 = sizeof(numsTrue) / sizeof(numsTrue[0]);
	printp("Testing array where condition should be true ('cannot add'):\n");
	duplicateNum(numsTrue, numSize1);

	printp("\n");

	int numsFalse[10] = {5, 1, 16, 78, 90, 55, 25, 54, 23, 20};
	int numSize2 = sizeof(numsFalse) / sizeof(numsFalse[0]);
	printp("Testing array where condition should be false (no output = success):\n");
	duplicateNum(numsFalse, numSize2);

	printp("\n");

	printp("Duplicate Number test complete.\n\n");
}

void twoSumTest() {

	printp("--TWO SUM TEST--\n\n");

	int nums[4] = {7, 2, 11, 15};
	int target = 9;
	char key[32], diffStr[32];
	int numSize = sizeof(nums) / sizeof(nums[0]);

	printp("Two Sum test w/ HashMap implementation:\n");
	printp("Input array: [7, 2, 11, 15]\nTarget: 9\nExpected indices: {0, 1}\nReturned indices: ");

	twoSum(nums, numSize, target, key, diffStr);

	printp("\n");
	printp("Two Sum test complete.\n");	


}



/*
 * Object Tests
 */

void stringTests() {
	
	printp("--STRING TEST--\n");

	printp("\n");

	char *string = "Hello, World!";
	printp("Original string: %s\n", string);

	printp("\n");

	printp("Replace All ('e' with 'w'):\n");
	strReplace(string, 'e', 'w');
	printp("%s\n", string);

	printp("\n");

	printp("To lower case:\n");
	strToLower(string);
	printp("%s\n", string);

	printp("\n");
	
	printp("To upper case:\n");
	strToUpper(string);
	printp("%s\n", string);

	printp("\n");
}
