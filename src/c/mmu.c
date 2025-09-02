#include "page.h"
#include "mmu.h"
#include "io.h"

// Page Table Entry Flags
#define PTE_VALID 	(1UL << 0)
#define PTE_TBL 	(1UL << 1)
#define PTE_AF 		(1UL << 10)
#define PTE_BLOCK 	(0UL << 1)
#define PTE_SH_OUTER	(2UL << 8)
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

// Exception Handler
extern void install_exception_vector(void);

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
unsigned long C_map_device_attrs() { return PTE_VALID | PTE_AF | PTE_SH_OUTER | PTE_AP_RW_EL1 | PTE_ATTR_IDX_1; }

void C_map_page(
	unsigned long *l1_tbl, 
	unsigned long vaddr, 
	unsigned long paddr, 
	unsigned long attrs
	) {
	unsigned long l1_idx = (vaddr >> 30) & 0x1FF,	// Top-level index (L1 - bits [38:30])
		      l2_idx = (vaddr >> 21) & 0x1FF,	// Second-level index (L2 - bits [29:21])
		      *l1_entry = &l1_tbl[l1_idx],	// Pointer to L1 entry
		      *l2_tbl;				// Undefined - Will hold pointer to L2 table
	// If L1 entry is invalid, allocate new L2 table 
	if ((*l1_entry & PTE_VALID) == 0) {		
		
		// Allocate a new page for L2 table and get its paddr
		struct ppage *l2_page = allocatePhysPages(1);
		l2_tbl = (unsigned long *)getPhysAddr(l2_page);
		
		// The L1 entry is added to the L2 table
		*l1_entry = ((unsigned long)l2_tbl & ~0xFFFUL) | PTE_VALID | PTE_TBL;
	
	// Otherwise, reuse existing L2 table
	} else 	l2_tbl = (unsigned long *)(*l1_entry & ~0xFFFUL);

	// Pointer to L2 entry corresponding to vaddr
	unsigned long *l2_entry = &l2_tbl[l2_idx];

	// Encode physical page address into entry + attributes
	unsigned long long mapped_val = (paddr & ~0x1FFFFFUL) | 0x705;
	
	// Write final mapping into L2 entry
	*l2_entry = (unsigned long)mapped_val;
}

void C_init_mmu(struct ppage *l1_page) {
	
	// Confirm L1 page isn't null
	if (l1_page == NULL) return;

	// This should probably go in kernel_main.c instead
	install_exception_vector();

	// EL1h check
	unsigned long spsel;
	asm volatile("mrs %0, SPSel" : "=r"(spsel));
	printp("SPSel = %d\n", spsel);

	unsigned long vbar;
	asm volatile("mrs %0, VBAR_EL1" : "=r"(vbar));
	printp("VBAR_EL1 = 0x%x\n", (unsigned int)vbar);

	// Define register values for MAIR, TCR, and TTBR0
	unsigned long mair_val = (0xFF << 0) | (0x04 << 8) | (0x44 << 16);
	unsigned long tcr_val =
	       (16UL << 0)	| // T0SZ: 16 = 48-bit VA space
	       (0b00UL << 6)	| // IRGN0: Inner cacheable normal memory
	       (0b00UL << 8)	| // ORGN0: Outer cacheable normal memory
	       (0b11UL << 12)	| // SH0: Shareable memory type for TTBR0
	       (0b0UL << 14);	  // TG0: Granule size for TTBR0, 0b0 = 4KB 

	unsigned long l1_phys = (unsigned long)getPhysAddr(l1_page);

	// Update registers w/ new values
	asm volatile("msr TCR_EL1, %0" :: "r"(tcr_val));
	asm volatile("msr MAIR_EL1, %0" :: "r"(mair_val));
	asm volatile("msr TTBR0_EL1, %0" :: "r"(l1_phys));
	
	asm volatile("dsb ish");
	asm volatile("isb");
	asm volatile("tlbi vmalle1; dsb ish; isb");
	
	// Define System Control Register (SCTLR) aka. Enable MMU
	unsigned long sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	
	;// Set mandatory/reserved bits
	sctlr |= 0xC00800;
	sctlr &= ~(
			(1 << 25) |	// EE - Endianness of translation tables (0 = little endian)
			(1 << 24) |	// E0E - Endianness of EL0 data access (0 = little endian)
        		(1 << 19) |	// WXN - Write XOR Execute (0 = allow writable + execute)
        		(1 << 12) |	// I - Instruction Cache enable (0 = disabled)
        		(1 << 4) |	// SA0 - Stack Alignment Check for EL0 (0 = disabled)
        		(1 << 3) |	// SA - Stack Alignment Check for EL1 (0 = disabled)
        		(1 << 2) |	// C - Data Cache enable (0 = disabled)
        		(1 << 1)	// A - Alignment Check enable (0 = disabled)
			);	
	
	// Enable MMU
	sctlr |=  (1 << 0);
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	asm volatile("isb");
	unsigned long new_sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(new_sctlr));

	printp("SCTLR_EL1 = 0x%x\n", (unsigned int)new_sctlr);
}
