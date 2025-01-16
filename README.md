# PiOS - Kernel Development in C

This repository, which began as a forked class project, has evolved to serve more as a personal sandbox for me to tinker with. From implementing terminal drivers, a page frame allocator, and common data structures/algorithms, the only real goal here is to learn. With that in mind, these are a few specific items that I intend to explore:

#### Implement Kernel Items:
1. [x] System timer
2. [x] Serial port mapping
3. [x] Custom print function
4. [x] Custom string library
5. [ ] Custom memory library
6. [ ] Custom test suite library
7. [x] Page frame allocator
8. [ ] Polling
9. [ ] Interrupts
10. [ ] Memory Management Unit (MMU)
11. [ ] Integrate FAT File System

#### Implement Common Data Structures:
1. [x] Linked List
2. [x] Stack
3. [x] Queue
4. [x] HashMap
5. [x] Vector
6. [x] Heap (min & max)
7. [ ] Binary Tree
8. [ ] Graph

#### Implement Search/Sort Algorithms:
1. [x] Bubble Sort
2. [x] Insertion Sort
3. [ ] Deletion Sort
4. [x] Quick Sort
5. [ ] Merge Sort
6. [ ] Bucket Sort
7. [x] Binary Search
8. [x] 2D Binary Search
9. [ ] Depth-First Search (DFS)
10. [ ] Breadth-First Search (BFS)

#### Coding Challenges w/ Data Structures:
1. HashMap
   * [x] Two Sum
3. HashSet
   * [x] Duplicate Number
4. Stack
   * [ ] Valid Parentheses
5. Linked List
   * [ ] Reverse Linked List
   * [ ] Linked List Cycle
   * [ ] Merge Two Sorted Lists
6. [ ] Binary Tree
   * [ ] Invert Tree
   * [ ] Maximum Depth of Tree
   * [ ] Balanced Tree, Same Tree
7. [ ] Heap
   * [ ] Last Stone Weight

#### Implement createProcess() "system call"
- Final project involved implementing a system call into the Linux kernel. I chose to focus on the Windows function, CreateProcess(), which combines fork() and exec(). The full paper on that can be found here: https://www.chrisso.tv

#### Perhaps make some small games that utilize the aforementioned data structures and algorithms?

Go ahead and poke around in the code! It's a bit messy but, to be honest, that's kind of the point.

Enjoy! - Chris
