#ifndef TEST_H
#define TEST_H

// Rust functions
extern void map_page(
		unsigned long long* l1_tbl,
		unsigned long long vaddr,
		unsigned long long paddr,
		unsigned long long attrs
		);
extern void init_mmu(unsigned long long *l1_tbl);
extern unsigned long long map_page_attrs(void);

// memory mapping
void print_page_tables(unsigned long long *l1);
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
