#include "page.h"
#include "io.h"

#define PAGE_SIZE (2 * 1024 * 1024)

struct ppage physPageArray[PAGE_COUNT];
struct ppage *freeList = NULL;

void init_pfa_list(void) {
	for (int i = 0; i < PAGE_COUNT; i++) {
		if (i < PAGE_COUNT - 1) {
			physPageArray[i].next = &physPageArray[i + 1]; // forward link
			physPageArray[i + 1].prev = &physPageArray[i]; // backward link
		}

		physPageArray[i].physAddr = (void *)(uintptr_t)(i * PAGE_SIZE); // test data value
	}
	physPageArray[0].prev = NULL; // first page has no previous
	physPageArray[PAGE_COUNT - 1].next = NULL; // last page has no next
	
	freeList = &physPageArray[0];
}

struct ppage *allocatePhysPages(unsigned int npages) {
	struct ppage *start = freeList;
	struct ppage *curr = freeList;

	if (npages == 0 || freeList == NULL) return NULL; // invalid allocation
	
	// traverse free list to find 'npages' consecutive pages
	for (unsigned int i = 0; i < npages; i++) {
		if (curr == NULL) return NULL; // not enough pages
		curr = curr->next;
	}

	// detach allocated pages from free list
	if (start == freeList) freeList = curr; // update head if we're allocating from head
	else if (start->prev) start->prev->next = curr; // link prev to next
	
	// if curr is not null, curr's previous node is now start's previous node
	if (curr) curr->prev = start->prev;

	start->prev = NULL;

	if ((uintptr_t)start->physAddr % PAGE_SIZE != 0) {
		printp("physAddr 0x%x is not page-aligned!\n", start->physAddr);
		return NULL;
	} else printp("physAddr 0x%x is page-aligned\n", start->physAddr);

	printp("Allocated %d pages. Starting physical address: 0x%x. New freeList head: 0x%x\n", npages, start->physAddr, freeList->physAddr);
		
	return start;

}

void freePhysPages(struct ppage *ppageList) {
	if (ppageList == NULL) return;

	// find the last page of the block being freed
	struct ppage *last = ppageList; // initialize pointer to input
	while (last->next != NULL) last = last->next; // iterate to end of list

	// check if the block is already part of the free list
	if (last->next == freeList || ppageList == freeList) {
		printp("Error: trying to free pages already in free list.\n");
		return;
	}

	// attach the freed list to front of the free list
	last->next = freeList;
	if (freeList != NULL) freeList->prev = last;

	// update the free list head
	freeList = ppageList;

	// ensure previous pointer of head is null
	freeList->prev = NULL;
	last->next = NULL;

	printp("Freed pages added. New freeList head: 0x%x\n", freeList->physAddr);
}

void printFreeList(void) {
	struct ppage *curr = freeList;
	int count = 0;
	if (curr == NULL) {
		printp("Free List is empty.\n");
		return;
	}	
	printp("Free List:\n");
	while (curr != NULL) {
		printp("[%d] Physical Address: 0x%x, prev: 0x%x, next: 0x%x\n", count, curr->physAddr, curr->prev->physAddr, curr->next->physAddr);
		if (count > PAGE_COUNT - 1) {
			printp("Error: Detected circular reference.\n");
			break;
		}
		curr = curr->next;
		count++;
	}
	printp("End of Free List (NULL)\n");
}

void *getPhysAddr(struct ppage *page) { return page->physAddr; }
