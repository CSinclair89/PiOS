#include "page.h"
#include "mmu.h"
#include "io.h"

void *mapPages(void *vaddr, struct ppage *page, struct page_directory *pd) {

	// calculate indices for page directory & table
	unsigned long vaddrNum = (unsigned long)vaddr;
	unsigned long pdIndex = (vaddrNum >> 21) & 0x3FF; // bits 31-22 for page dir
	
	// access page directory entry
	struct page_directory_entry *pdEntry = &pd->entries[pdIndex];

	// populate page table entries
	pdEntry->frame = (unsigned long)page->physAddr >> 21;
	pdEntry->pagesize = 1;
	pdEntry->present = 1;
	pdEntry->rw = 1;
	pdEntry->user = 1;

	printp("Mapped vaddr 0x%x to paddr 0x%x\n", vaddr, page->physAddr);

	// return virtual address
	return vaddr;
}

void *mapAddress(void *vaddr, void *paddr) { return paddr; }
