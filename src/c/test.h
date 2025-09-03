#ifndef TEST_H
#define TEST_H

// Rust functions
extern void rs_map_page(
		unsigned long *l1_tbl,
		unsigned long vaddr,
		unsigned long paddr,
		unsigned long attrs
		);
extern void rs_init_mmu(unsigned long *l1_tbl);
extern unsigned long map_page_attrs(void);
extern unsigned long map_device_attrs(void);
extern unsigned long rs_page_desc_norm(void);
extern void call_print_paddr(void *paddr);

// memory mapping
void print_page_tables_before_MMU(unsigned long long *l1);
void print_page_tables_after_MMU(unsigned long long *l1);
void mmuTests();

// data structures
void listTests();
void mapTests();
void queueTests();
void stackTests();
void setTests();
void heapTests();
void treeTests();
void vectorTests();

// algorithms
void arraySortTests();
void arraySearchTests();
void duplicateNumTest();
void twoSumTest();

// objects
void stringTests();
void quirksTest();
void randTest();

#endif
