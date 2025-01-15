#ifndef ALGOS_H
#define ALGOS_H

/*
 * Array Sort
 */

void bubbleSortAsc(int *nums, int numSize);
void bubbleSortDesc(int *nums, int numSize);
void insertionSortAsc(int *nums, int numSize);
void insertionSortDesc(int *nums, int numSize);
void quickSort(int * nums, int numSize);

/*
 * Array Search
 */

/*
 * HashSet
 */

void duplicateNum(int *nums, int numSize);

/*
 * HashMap
 */

int* twoSum(int *nums, int numSize, int target, char *key, char *diffStr);

#endif
