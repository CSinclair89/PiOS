#include "ds.h"
#include "mmu.h"
#include "string.h"
#include "algos.h"
#include "tree.h"
#include "page.h"
#include "mem.h"
#include "io.h"
#include "math.h"

// mapping

void print_page_tables_before_MMU(unsigned long long *l1) {

	for (int i = 0; i < 512; i++) {
		if (l1[i] & 1) {
			putp('L'); putp('1'); putp('[');
			putu(i);
			putp(']');
			putp('=');
			puthex(l1[i]);
			putp('\n');

			// If L1 points to a table, walk L2
			if ((l1[i] & 0x3) == 0x3) {
				unsigned long long *l2 = (unsigned long long *)(l1[i] & ~0xFFFUL);
				for (int j = 0; j < 512; j++) {
					if (l2[j] & 1) {
						putp(' '); putp('L'); putp('2'); putp('[');
						putu(j);
						putp(']');
						putp('=');
						puthex(l2[j]);
						putp('\n');
					}
				}
			}
		} // else putp('x');
	}
}

void print_page_tables_after_MMU(unsigned long long *l1) {

	for (int i = 0; i < 512; i++) {
		if (l1[i] & 1) {
			putp('L'); putp('1'); putp('[');
			putu(i);
			putp(']');
			putp('=');
			puthex((unsigned int)(l1[i] >> 32));
			puthex((unsigned int)(l1[i] & 0xFFFFFFFF));
			putp('\n');

			// If L1 points to a table, walk L2
			if ((l1[i] & 0x3) == 0x3) {
				unsigned long long *l2 = (unsigned long long *)(l1[i] & ~0xFFFUL);
				for (int j = 0; j < 512; j++) {
					if (l2[j] & 1) {
						putp(' '); putp('L'); putp('2'); putp('[');
						putu(j);
						putp(']');
						putp('=');
						puthex((unsigned int)(l2[j] >> 32));
						puthex((unsigned int)(l2[j] & 0xFFFFFFFF));
						putp('\n');
					}
				}
			}
		}
	}
}

__attribute__((aligned(16))) static char el0_stack[0x1000];
__attribute__((aligned(16))) static char el1_stack[0x1000];

void el0_test() {
	asm volatile(
			"svc #0\n"
			"brk #0\n"
		    );
}

void to_el0() {
	// Set EP_EL0 to top of el0_stack
	asm volatile("msr SP_EL0, %0" :: "r"(el0_stack + sizeof(el0_stack)));

	// Prepare SPSR_EL1 for EL0t
	unsigned long spsr_el1 = 0;
	spsr_el1 &= ~(0b1111);		// Clear M[3:0] bits (Exception level)
	spsr_el1 |= 0b0000;		// Set M[3:0] = 0b0000 (EL0t mode)
	spsr_el1 &= ~(1 << 6);		// Clear F (FIQ Interrupt disable)
	spsr_el1 &= ~(1 << 7);		// Clear I (IRQ Interrupt disable)
	spsr_el1 &= ~(1 << 8);		// Clear A (SError Interrupt disable)
	asm volatile("msr SPSR_EL1, %0" :: "r"(spsr_el1));

	// Set ELR_EL1 to EL0_test
	asm volatile("msr ELR_EL1, %0" :: "r"(el0_test));

	// Drop to EL0
	asm volatile("eret");
}

void mmuTests() {	
	printp("--MMU TEST--\n\n");
	init_pfa_list();
	freeList = &physPageArray[20];
	struct ppage *l1_page = allocatePhysPages(1);
	unsigned long *l1_tbl = (unsigned long *)getPhysAddr(l1_page);
	for (int i = 0; i < 512; i++) l1_tbl[i] = 0;
	
	// EL1h check
        unsigned long spsel;
        asm volatile("mrs %0, SPSel" : "=r"(spsel));
        printp("SPSel = %d\n", spsel);

	// Current EL check
	unsigned long current_el;
	asm volatile("mrs %0, CurrentEL" : "=r"(current_el));
	printp("CurrentEL = %x\n", current_el >> 2);

	// Check VBAR_EL1 at Runtime
	unsigned long vbar;
	asm volatile("mrs %0, VBAR_EL1" : "=r"(vbar));
	printp("VBAR_EL1 before install_exception_vector() = 0x%x\n",(unsigned int)vbar);

	install_exception_vector();

	asm volatile("mrs %0, VBAR_EL1" : "=r"(vbar));
	printp("VBAR_EL1 after install_exception_vector() = 0x%x\n", (unsigned int)vbar);
/*
	// Drop to EL0
	printp("Dropping to EL0\n");
	to_el0();
	printp("Return from EL0\n");
*/
/*
	// Test SVC
	printp("Triggering SVC\n");
	asm volatile("svc #0");
	printp("Returned from SVC\n");
*/
	/*
	 *  C Identity Mapping Test
	 */

	// C Identity Mapping Test
	unsigned long id_vaddr = 0x2A00000UL;
	unsigned long page_desc_norm = C_page_desc_norm();
	C_map_page(l1_tbl, id_vaddr, 0x2A00000UL, page_desc_norm);

	// C Identity Mapping VBAR_EL1
	unsigned long vbar_vaddr = 0x80800;
	C_map_page(l1_tbl, vbar_vaddr, 0x80800, page_desc_norm);

	// C Identity Mapping Exception Vector Table
	unsigned long evt_vaddr = 0x80000;
	C_map_page(l1_tbl, evt_vaddr, 0x80000, page_desc_norm);
	// C Non-Identity Mapping Test
	struct ppage *nonid_page = allocatePhysPages(1);
	unsigned long nonid_paddr = (unsigned long)getPhysAddr(nonid_page);
	unsigned long nonid_vaddr = 0x2C0000UL;
	C_map_page(l1_tbl, nonid_vaddr, nonid_paddr, page_desc_norm);

	// C Serial Port Mapping Test
	unsigned long serial_paddr = 0x3F200000UL;
	unsigned long serial_vaddr = 0x20000000UL;
	C_map_page(l1_tbl, serial_vaddr, serial_paddr, C_map_device_attrs());
	unsigned long serial_offset = 0x15040UL;
	printp("Serial offset: 0x%x\n", serial_offset);
	printp("Paddr + serial offset: 0x%x\n", serial_paddr + serial_offset);
	volatile unsigned char *serial_dr = (unsigned char *)(serial_vaddr + serial_offset);
	printp("Vaddr + serial offset: 0x%x\n", serial_dr);

	// Rust Identity Mapping Test
	struct ppage *rs_id_page = allocatePhysPages(1);
	unsigned long rs_id_paddr = getPhysAddr(rs_id_page);

	print_page_tables_before_MMU(l1_tbl);
	C_init_mmu(l1_page);

	asm volatile(
			"dsb ish\n"
			"tlbi vmalle1\n"
			"dsb ish\n"
			"isb\n"
		    );

	// C Identity Mapping Test
	*(unsigned int *)id_vaddr = 0xDEADBEEF;
	unsigned int id_read = *(unsigned int *)id_vaddr;
	printp("Identity Mapping Test: 0x%x\n", id_read);
	
	// C Non-Identity Mapping Test
	*(unsigned int *)nonid_vaddr = 0xCAFEBABE;
	unsigned int nonid_read = *(unsigned int *)nonid_vaddr;
	printp("Non-Identity Mapping Test: 0x%x\n", nonid_read);

	// C Unmapped Access Test
/*
	unsigned long unmapped_vaddr = 0x40000000UL;
	printp("Attempting to access unmapped_vaddr 0x%x\n", unmapped_vaddr);
	printp("test1\n");
	*(unsigned int *)unmapped_vaddr = 0xAABBCCDD;
	printp("test2\n");
	unsigned int unmapped_read = *(unsigned int*)unmapped_vaddr;
	printp("Unmapped Access Test: 0x%x\n", unmapped_read);

*/
	// Serial Port Mapping Test
	// step 1 non-id test
/*
	*(unsigned int *)serial_vaddr = 0xAABBCCDD;
	unsigned int serial_read = *(unsigned int *)serial_vaddr;
	printp("Serial Read: 0x%x\n", serial_read);
*/
	printp("serial_dr addr: 0x%x\n", serial_dr);
	*serial_dr = 'H';
	asm volatile("dsb sy" ::: "memory");
	printv("Hello\n");

/*
	// Test SVC
	printp("Triggering SVC\n");
	asm volatile("svc #0");
	printp("Returned from SVC\n");	
*/


	unsigned long esr, far, mair;
	asm volatile("mrs %0, ESR_EL1" : "=r"(esr));
	asm volatile("mrs %0, FAR_EL1" : "=r"(far));
	asm volatile("mrs %0, MAIR_EL1" : "=r"(mair));
	printp("ESR_EL1 = 0x%x\n", (unsigned int)esr);
	printp("FAR_EL1 = 0x%x\n", (unsigned int)far);
	printp("MAIR_EL1 = 0x%x%08x\n", 
			(int)(mair >> 32),
			(int)(mair & 0xFFFFFFFF));
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

	printp("--ARRAY SORTING TESTS--\n\n");

	// Bubble Sort
	int nums1[10] = {8, 91, 67, 34, 52, 1, 50, 66, 17, 64};
	int numSize = sizeof(nums1) / sizeof(nums1[0]);
	
	printp("Testing Bubble Sort (with goto) w/ unsorted array:\n");
	arrayToString(nums1, numSize);	
	
	printp("Array after Bubble Sort (ascending):\n");
	bubbleSort(nums1, numSize, "asc");
	printp("[%d", nums1[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums1[i]);
	printp("]\n\n");

	printp("Array after Bubble Sort (descending):\n");
	bubbleSort(nums1, numSize, "desc");
	printp("[%d", nums1[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums1[i]);
	printp("]\n\n");

	printp("Array after Bubble Sort (testing error msg):\n");
	bubbleSort(nums1, numSize, "asdfasdf");

	printp("\n");

	// Insertion Sort
	int nums2[10] = {8, 91, 67, 34, 52, 1, 50, 66, 17, 64};

	printp("Testing Insertion Sort (with goto) w/ unsorted array:\n");
	printp("[%d", nums2[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums2[i]);
	printp("]\n");

	printp("\n");

	printp("Array after Insertion Sort (ascending):\n");
	insertionSort(nums2, numSize, "asc");
	printp("[%d", nums2[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums2[i]);
	printp("]\n");

	printp("\n");

	printp("Array after Insertion Sort (descending):\n");
	insertionSort(nums1, numSize, "desc");
	printp("[%d", nums1[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums1[i]);
	printp("]\n\n");

	printp("Array after Insertion Sort (testing error msg):\n");
	bubbleSort(nums2, numSize, "asdfasdf");

	printp("\n");

	// Quick Sort
	int nums3[10] = {7, 9, 6, 54, 52, 171, 550, 366, 217, 4};

	printp("Testing Quick Sort (with goto) w/ unsorted array:\n");
	printp("[%d", nums3[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums3[i]);
	printp("]\n\n");

	printp("Array after Quick Sort (ascending):\n");
	quickSort(nums3, 0, numSize - 1, "asc");
	printp("[%d", nums3[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums3[i]);
	printp("]\n\n");

	printp("Array after Quick Sort (descending):\n");
	quickSort(nums3, 0, numSize - 1, "desc");
	printp("[%d", nums3[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums3[i]);
	printp("]\n\n");

	printp("Array after Quick Sort (testing error msg):\n");
	quickSort(nums3, 0, numSize - 1, "asdfasdf");

	printp("\n");

	// Selection Sort
	int nums4[10] = {52, 917, 34, 32, 14, 30, 69, 117, 634, 54};

	printp("Selection Sort test, initial array:\n");
	printp("[%d", nums4[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums4[i]);
	printp("]\n\n");

	printp("Selection Sort test, array after sort ascending:\n");
	selectionSort(nums4, numSize, "asc");
	printp("[%d", nums4[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums4[i]);
	printp("]\n\n");

	printp("Selection Sort test, array after sort descending:\n");
	selectionSort(nums4, numSize, "desc");
	printp("[%d", nums4[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums4[i]);
	printp("]\n\n");

	printp("Selection sort test, array after triggering error msg:\n");
	selectionSort(nums4, numSize, "aasdfasdf");

	printp("\n");

	// Bogo Sort
	int nums5[10] = {13, 431, 47, 14, 502, 17, 20, 42, 56, 61};
	
	
	printp("Bogo Sort test, initial array:\n");
	printp("[%d", nums5[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums5[i]);
	printp("]\n\n");

	printp("Bogo Sort test, array after sort ascending:\n");
	bogoSort(nums5, numSize, "asc");
	printp("[%d", nums5[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums5[i]);
	printp("]\n\n");

	printp("Bogo Sort test, array after sort descending:\n");
	bogoSort(nums5, numSize, "desc");
	printp("[%d", nums5[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums5[i]);
	printp("]\n\n");

	printp("Bogo sort test, array after triggering error msg:\n");
	bogoSort(nums5, numSize, "aasdfasdf");

//	int nums6[10] = {85, 1, 63, 354, 2, 1, 87, 13, 15, 646};
//	int nums7[10] = {66, 4, 32, 34, 532, 10, 58, 31, 98, 2};

}

void arraySearchTests() {
	
	printp("--ARRAY SEARCH TESTS--\n\n");

	int nums1[10] = {8, 91, 67, 34, 52, 1, 50, 66, 17, 64};
	int numSize = sizeof(nums1) / sizeof(nums1[0]);
	bubbleSort(nums1, numSize, "asc");

	printp("Binary Search -- looking for element %d in ascending array (present):\n", 34);
	printp("[%d", nums1[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums1[i]);
	printp("]\n\n");

	binarySearch(nums1, numSize, 34);

	printp("\n");

	printp("Binary Search -- looking for element %d in ascending array (NOT present):\n\n", 92);

	binarySearch(nums1, numSize, 92);
	
	bubbleSort(nums1, numSize, "desc");

	printp("\n");

	printp("Binary Search -- looking for element %d in descending array (present):\n", 34);
	printp("[%d", nums1[0]);
	for (int i = 1; i < numSize; i++) printp(", %d", nums1[i]);
	printp("]\n\n");


	binarySearch(nums1, numSize, 34);

	printp("\n");

	printp("Binary Search -- looking for element %d in descending array (NOT present):\n\n", 92);

	binarySearch(nums1, numSize, 92);

	printp("\n");

	int nums2[3][10] = {
		{8, 91, 67, 34, 52, 1, 50, 66, 17, 64}, 
		{16, 81, 96, 103, 3, 67, 15, 89, 77, 84}, 
		{99, 104, 13, 42, 78, 63, 56, 11, 15, 75}
	};

	int rows = 3;
	int cols = 10;

	printp("2D array before sorting: \n");
	array2DToString(nums2, 3, 10);

	printp("\n");

	// sorting each row
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols ; j++) bubbleSort(nums2[i], cols, "asc");
	}

	printp("2D array after sorting: \n");
	array2DToString(nums2, 3, 10);

	printp("\n");

	binarySearch2D(nums2, rows, cols, 16);

	printp("\n");	
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

	printp("String concatenate:\n");
	char str1[50] = "Hello, ";
	char str2[] = "world!";
	char *newStr = strCat(str1, str2);
	printp("%s\n", newStr);
}

void randTest() {
	printp("--RANDOM NUMBER TESTS--\n");
	srand(123);
	int r1 = rand();
        int r2 = rand();
        int r3 = rand();
	printp("Random numbers: %d, %d, %d\n", r1, r2, r3);

	int r4 = rand() % 10 + 1;
	int r5 = rand() % 10 + 1;
	int r6 = rand() % 10 + 1;
	int r7 = rand() % 10 + 1;
	printp("Testing 'rand() % 10': %d, %d, %d, %d\n", r4, r5, r6, r7);

	int r8 = randRange(7, 14);
	int r9 = randRange(50, 100);
	int r10 = randRange(200, 500);
	printp("Testing rangdRange: %d, %d, %d\n", r8, r9, r10);
}
