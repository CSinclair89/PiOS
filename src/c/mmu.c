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
#define PAGE_DESC_NORMAL ((PTE_VALID | PTE_AF | PTE_ATTR_IDX_1 | PTE_AP_RW_EL1 | PTE_SH_INNER))
#define PAGE_DESC_NC ((PTE_VALID | PTE_AF | PTE_ATTR_IDX_1 | PTE_AP_RW_EL1 | PTE_SH_INNER | UXN | PXN))

// Execute-never Flags
#define UXN 		(1ULL << 54)
#define PXN 		(1ULL << 53)

// Memory Attribute Indirection Register Attribute Values
#define MAIR_IDX0_NORMAL 0xFFUL
#define MAIR_EL1_VAL (MAIR_IDX0_NORMAL << (0 * 8))
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

unsigned long C_page_desc_norm() { return PAGE_DESC_NORMAL; }
unsigned long C_page_desc_nc() { return PAGE_DESC_NC; }
unsigned long C_map_device_attrs() { return PTE_ATTR_IDX_1 | UXN | PXN; }

void C_map_page(
	unsigned long *l1_tbl, 
	unsigned long vaddr, 
	unsigned long paddr, 
	unsigned long attrs
	) {
	unsigned long l1_idx = (vaddr >> 30) & 0x1FF,
		      l2_idx = (vaddr >> 21) & 0x1FF,
		      *l1_entry = &l1_tbl[l1_idx],
		      *l2_tbl;
	if ((*l1_entry & PTE_VALID) == 0) {
		struct ppage *l2_page = allocatePhysPages(1);
		l2_tbl = (unsigned long *)getPhysAddr(l2_page);
		*l1_entry = ((unsigned long)l2_tbl & ~0xFFFUL) | PTE_VALID | PTE_TBL;
	} else 	l2_tbl = (unsigned long *)(*l1_entry & ~0xFFFUL);
	unsigned long *l2_entry = &l2_tbl[l2_idx];
	unsigned long long mapped_val = (paddr & ~0x1FFFFFUL) | 0x705;
	*l2_entry = (unsigned long)mapped_val;
}

void C_init_mmu(struct ppage *l1_page) {
	if (l1_page == NULL) return;
	unsigned long l1_phys = (unsigned long)getPhysAddr(l1_page);
	unsigned long mair_val = (0xFF << 0) | (0x04 << 8) | (0x44 << 16);
	asm volatile("msr MAIR_EL1, %0" :: "r"(mair_val));
	unsigned long tcr_val =
	       (16UL << 0)	| // T0SZ: 16 = 48-bit VA space
	       (0b00UL << 6)	| // IRGN0: Inner cacheable normal memory
	       (0b00UL << 8)	| // ORGN0: Outer cacheable normal memory
	       (0b11UL << 12)	| // SH0: Shareable memory type for TTBR0
	       (0b0UL << 14);	  // TG0: Granule size for TTBR0, 0b00 = 4KB 

/*
	unsigned long b = 0xF;
	unsigned long tcr_val =  (0b00LL << 37) | // TBI=0, no tagging
        (b << 32) |      // IPS=autodetected
        (0b10LL << 30) | // TG1=4k
        (0b11LL << 28) | // SH1=3 inner
        (0b01LL << 26) | // ORGN1=1 write back
        (0b01LL << 24) | // IRGN1=1 write back
        (0b0LL  << 23) | // EPD1 enable higher half
        (25LL   << 16) | // T1SZ=25, 3 levels (512G)
        (0b00LL << 14) | // TG0=4k
        (0b11LL << 12) | // SH0=3 inner
        (0b01LL << 10) | // ORGN0=1 write back
        (0b01LL << 8) |  // IRGN0=1 write back
        (0b0LL  << 7) |  // EPD0 enable lower half
        (25LL   << 0);   // T0SZ=25, 3 levels (512G)
*/
	asm volatile("msr TCR_EL1, %0" :: "r"(tcr_val));
	asm volatile("msr TTBR0_EL1, %0" :: "r"(l1_phys));
	asm volatile("dsb ish");
	asm volatile("isb");
	asm volatile("tlbi vmalle1; dsb ish; isb");
	unsigned long sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr |= 0xC00800;     // set mandatory reserved bits
	sctlr &= ~((1 << 25) |   // clear EE, little endian translation tables
	(1 << 24) |   // clear E0E
        (1 << 19) |   // clear WXN
        (1 << 12) |   // clear I, no instruction cache
        (1 << 4) |    // clear SA0
        (1 << 3) |    // clear SA
        (1 << 2) |    // clear C, no cache at all
        (1 << 1));    // clear A, no alignment check
	sctlr |=  (1 << 0);     // set M, enable MMU
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	asm volatile("isb");
	unsigned long new_sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(new_sctlr));
	printp("SCTLR_EL1 = 0x%x\n", (unsigned int)new_sctlr);
}
