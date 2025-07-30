#ifndef PAGE_H
#define PAGE_H

#define NULL (void *)0
#define PAGE_COUNT 2048
#define uintptr_t unsigned long long

struct ppage {
	struct ppage *next;
	struct ppage *prev;
	void *physAddr;
};

extern struct ppage physPageArray[PAGE_COUNT];
extern struct ppage *freeList;

void init_pfa_list(void);
struct ppage *allocatePhysPages(unsigned int npages);
void freePhysPages(struct ppage *ppageList);
void printFreeList(void);
void *getPhysAddr(struct ppage *page);
void cleanPageCache(void *addr);

#endif
