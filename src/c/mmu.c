#include "page.h"
#include "mmu.h"
#include "io.h"

// Page Table Entry Flags
#define PTE_VALID 	(1UL << 0)
#define PTE_TBL 	(1UL << 1)
#define PTE_AF 		(1UL << 10)
#define PTE_BLOCK 	(0UL << 1)
#define PTE_SH_INNER 	(3UL << 8)
#define PTE_AP_RW_EL1 	(0UL << 6)
#define PTE_ATTR_IDX_0 	(0UL << 2)

// Execute-never Flags
#define UXN 		(1ULL << 54)
#define PXN 		(1ULL << 53)

// Memory Attribute Indirection Register Attribute Values
#define MAIR_NORMAL	0xFF
#define MAIR_DEVICE_NGNRE 0x04

int C_map_device_attrs() {
	PTE_ATTR_IDX_0 | UXN | PXN;
}

int C_map_page(
	unsigned long *l1_tbl, 
	unsigned long vaddr, 
	unsigned long paddr, 
	unsigned long attrs
	) {

	unsigned long l1_idx = (vaddr >> 30) & 0x1FF,
		      l2_idx = (vaddr >> 21) & 0x1FF;

	printp("map_page() start\n");

	printp("l1_idx: 0x%x, l2_idx: 0x%x\n", 
			(unsigned int)l1_idx, 
			(unsigned int)l2_idx);

	unsigned long *l1_entry = &l1_tbl[l1_idx];
	unsigned long *l2_tbl;

	if ((*l1_entry & PTE_VALID) == 0) {

		printp("L1 entry not valid, allocating L2 table...\n");

		struct ppage *l2_page = allocatePhysPages(1);
		unsigned long l2_paddr = (unsigned long)l2_page;

		printp("l2_page = 0x%x%08x\n", 
				(unsigned int)(l2_paddr >> 32),
				(unsigned int)(l2_paddr & 0xFFFFFFFF));

		l2_tbl = (unsigned long *)getPhysAddr(l2_page);
		unsigned long tbl_phys = (unsigned long)l2_tbl;

		printp("l2_tbl = 0x%x\n", tbl_phys);

		*l1_entry = ((unsigned long)l2_tbl & ~0xFFFUL) | PTE_VALID | PTE_TBL;

		printp("L1 entry updated\n");
	} else {
		printp("L1 entry already valid, using existing L2 table\n");
		l2_tbl = (unsigned long *)(*l1_entry & ~0xFFFUL);
	}

	unsigned long *l2_entry = &l2_tbl[l2_idx];

	unsigned long long mapped_val = 
		((unsigned long long)(paddr & ~0x1FFFFFUL))
		| PTE_VALID
		| PTE_AF
		| PTE_ATTR_IDX_0
		| PTE_AP_RW_EL1
		| PTE_SH_INNER
		| UXN
		| PXN;

	printp("Writing L2[0x%x] = 0x%x%08x\n",
			(unsigned int)l2_idx,
			(unsigned int)(mapped_val >> 32),
			(unsigned int)(mapped_val & 0xFFFFFFFF));

	*l2_entry = (unsigned long)mapped_val;

	printp("completed map_page()\n");
}

/*
void C_init_mmu(*l1_tbl) {
	if (l1_tbl == NULL) return;

	unsigned long mair_val = 0x00FF;
	asm("msr MAIR_EL1, {}", mair_val);

	unsigned long tcr_val = 
	asm("msr TCR_EL1, {}", tcr_val);
	asm("msr TTBR0_EL1, {}", (unsigned long)l1_tbl);

	asm("dsb ish");
	asm("isb");

	unsigned long sctlr_el1;
	asm(
		"mrs {val}, SCTLR_EL1",
		"orr {val}, {val}, {bit}",
		"msr SCTLR_EL1, {val}",
		sctlr_el1, 1
	   );

	unsigned long new_sctlr;
	asm("mrs {}, SCTLR_EL1", new_sctlr);

	printp("SCTLR_EL1 = 0x%x\n", new_sctlr);

}

*/



/*
 * OLD FUNCTIONS
 *
 *
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
*
*/
