#include "page.h"
#include "io.h"

#define PAGE_SIZE 0x200000UL
#define CACHE_LINE_SIZE 64

struct ppage physPageArray[PAGE_COUNT];
struct ppage *freeList = NULL;

void init_pfa_list(void) {
	
	extern char __end;
	uintptr_t kernel_end = (uintptr_t)&__end;

	uintptr_t base = (kernel_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

	for (int i = 0; i < PAGE_COUNT; i++) {

		physPageArray[i].physAddr = (void *)(base + i * PAGE_SIZE);

		if (i < PAGE_COUNT - 1) {
			physPageArray[i].next = &physPageArray[i + 1]; // forward link
			physPageArray[i + 1].prev = &physPageArray[i]; // backward link
		}
/*
		physPageArray[i].physAddr = (void *)(uintptr_t)(i * PAGE_SIZE); // test data value
		
		if (((uintptr_t)physPageArray[i].physAddr & 0x1FFFFF) != 0) {
			printp("init_pfa_list: physAddr[%d] = 0x%x NOT 2MB aligned!\n", i, (unsigned int)(uintptr_t)physPageArray[i].physAddr);
		}
*/
	}
	physPageArray[PAGE_COUNT - 1].next = NULL; // last page has no next
	physPageArray[0].prev = NULL;
	freeList = &physPageArray[0];
	printp("Init PFA[0] addr = 0x%x\n", physPageArray[0].physAddr);
//	physPageArray[1].prev = NULL;
}

struct ppage *allocatePhysPages(unsigned int npages) {	
	if (npages == 0 || freeList == NULL) return NULL;
	struct ppage *curr = freeList;
	if ((uintptr_t)curr->physAddr % PAGE_SIZE != 0) printp("Misalignment.\n");
	while (curr) {
		if ((uintptr_t)curr->physAddr == 0x0) {
			curr = curr->next;
			continue;
		}
		// check for 512-page alignment first
		if (npages == 512 && (((uintptr_t)curr->physAddr & 0x1FFFFF) != 0)) {
			curr = curr->next;
			continue;
		}
		struct ppage *runner = curr;
		unsigned int count = 1;
		while (runner->next && count < npages && runner->next == runner + 1) {
			runner = runner->next;
			count++;
		}
		if (count == npages) {
			// detach allocated block from free list
			if (curr == freeList) {
				freeList = runner->next;
				if (freeList) freeList->prev = NULL;
			} else {
				if (curr->prev) curr->prev->next = runner->next;
				if (runner->next) runner->next->prev = curr->prev;
			}
			curr->prev = NULL;
			runner->next = NULL;
			if ((uintptr_t)curr->physAddr % PAGE_SIZE != 0) {
				printp("physAddr 0x%x is NOT page-aligned!\n", curr->physAddr);
				return NULL;
			} else printp("physAddr0x%x is page-aligned\n", curr->physAddr);
			printp("Allocated %d pages. Starting paddr: 0x%x. New freeList head: 0x%x\n", npages, curr->physAddr, freeList ? freeList->physAddr : 0);
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
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
		printp("[%d] Physical Address: 0x%x, prev: 0x%x, next: 0x%x\n",
				count,
				curr->physAddr,
				curr->prev ? (unsigned int)(uintptr_t)curr->prev->physAddr : 0,
				curr->next ? (unsigned int)(uintptr_t)curr->next->physAddr : 0);
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

void cleanPageCache(void *addr) {
	unsigned long start = (unsigned long)addr;
	unsigned long end = start + PAGE_SIZE;

	for (unsigned long i = start; i < end; i += CACHE_LINE_SIZE) {
		asm volatile (
				"dc civac, %0\n"
				:
				: "r" (i)
				: "memory"
			     );
	}

	asm volatile (
			"dsb sy\n"
			"isb\n"
		     );
}
