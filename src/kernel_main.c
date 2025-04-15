#include "timer.h"
#include "page.h"
#include "mmu.h"
#include "test.h"
#include "string.h"
#include "io.h"

char glbl[128];
// extern struct table_descriptor_stage1 L1table[512];

void kernel_main() {
    
	// Initialize bss segment variables and respective pointers
  	extern char __bss_start, __bss_end;
	char *bssStart, *bssEnd;  
	
	// Define bssStart and bssEnd variables
	bssStart = &__bss_start;
	bssEnd = &__bss_end;
	char *i = bssStart;
	
	// Set bss segment to 0
	for (; i < bssEnd; i++) *i = 0;

/*
 * Begin Page Frame Allocator
 */
	
/*	// initialize page frame allocator	
	init_pfa_list();

	// test print of pfa list
	printp("\nInitial Page Frame Allocation list");

	// define free list
	freeList = &physPageArray[0];

	// test print initial state
	printFreeList();

	// allocate 10 pages and print again
	struct ppage *allocatedPages = allocatePhysPages(10);
	printp("\nAllocated 10 pages:\n");
	printFreeList();

	// free the pages and print one last time
	freePhysPages(allocatedPages);
	printp("\nFreed the previous 10 pages:\n");
	printFreeList();
*/	

/*
 * End Page Frame Allocator
 */
/*
	printp("Data Structure Tests\n\n");
	listTests();
	printp("\n");
	mapTests();
	printp("\n");
	queueTests();
	printp("\n");
	stackTests();
	printp("\n");
	setTests();
	printp("\n");
	heapTests();
	printp("\n");
	treeTests();
	printp("\n");

	vectorTests();
*/

	printp("Algorithm Tests\n\n");
	arraySortTests();
	printp("\n");
	arraySearchTests();
	printp("\n");

/*
	duplicateNumTest();
	printp("\n");
*/
/*
	twoSumTest();
	printp("\n");
*/
/*
	printp("Object Tests\n\n");
	stringTests();
	printp("\n");
	quirksTest();
	printp("\n");
	randTest();
	printp("\n");
*/
/*
	printp("MMU Test\n\n");
	mmuTests();
	printp("\n");
*/



	// test system timer and reminder to terminate program
	waitFor(1000000);
	printp("\nPress Ctrl + C to terminate the signal...\n");

	while(1){
    }
}
