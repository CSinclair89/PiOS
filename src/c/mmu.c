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
#define PTE_ATTR_IDX_1 	(1UL << 2)

// Execute-never Flags
#define UXN 		(1ULL << 54)
#define PXN 		(1ULL << 53)

// Memory Attribute Indirection Register Attribute Values
#define MAIR_NORMAL	0xFF
#define MAIR_DEVICE_NGNRE 0x04

unsigned long C_map_attrs() {
	return PTE_AF 
		| PTE_SH_INNER
		| PTE_AP_RW_EL1 
		| PTE_ATTR_IDX_0 
		| UXN 
		| PXN; 
}

unsigned long C_map_device_attrs() { return PTE_ATTR_IDX_1 | UXN | PXN; }

void C_map_page(
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


void C_init_mmu(unsigned long *l1_tbl) {
	if (l1_tbl == NULL) return;

	unsigned long mair_val = (MAIR_NORMAL << 0) | (MAIR_DEVICE_NGNRE << 8);
	asm volatile("msr MAIR_EL1, %0" :: "r"(mair_val));

	unsigned long tcr_val = 
		  (16UL << 0)
		| (0b00UL << 6)
		| (0b00UL << 8)
		| (0b11UL << 12)
		| (0b0UL << 14);

	asm volatile("msr TCR_EL1, %0" :: "r"(tcr_val));

	asm volatile("msr TTBR0_EL1, %0" :: "r"(l1_tbl));

	asm volatile("dsb ish");
	asm volatile("isb");

	unsigned long sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr));

	sctlr |= (1 << 0);
	sctlr |= (1 << 2);
	sctlr |= (1 << 12);

	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	asm volatile("isb");

	unsigned long new_sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(new_sctlr));

	printp("SCTLR_EL1 = 0x%x\n", (unsigned int)new_sctlr);

}





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
