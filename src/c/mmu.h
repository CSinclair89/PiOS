#ifndef MMU_H
#define MMU_H

#include "page.h"

#define NUM_PAGES 128
#define PAGE_SIZE_2MB 0x200000
#define PAGE_MASK_2MB (~(PAGE_SIZE_2MB - 1))

unsigned long C_map_device_attrs();
void C_map_page(
		unsigned long *l1_tbl, 
		unsigned long vaddr, 
		unsigned long paddr, 
		unsigned long attrs
		);
void C_init_mmu(unsigned long *l1_tbl);

struct page_directory_entry {
   unsigned long present       : 1;   // Page present in memory
   unsigned long rw            : 1;   // Read-only if clear, R/W if set
   unsigned long user          : 1;   // Supervisor only if clear
   unsigned long writethru     : 1;   // Cache this directory as write-thru only
   unsigned long cachedisabled : 1;   // Disable cache on this page table?
   unsigned long accessed      : 1;   // Has the page been accessed since last refresh
   unsigned long reserved      : 1;   // Reserved for future use
   unsigned long pagesize      : 1;   // Use large 2MB pages
   unsigned long ignored       : 1;   // Ignored bits
   unsigned long os_specific   : 3;   // OS-specific bits
   unsigned long frame         : 20;  // Frame address (shifted right 12 bits)
};

struct page_directory {
	struct page_directory_entry entries[1024];
};

void *mapPages(void *vaddr, struct ppage *page, struct page_directory *pd);
void *mapAddress(void *vaddr, void *paddr);

#endif
