#ifndef ALGOS_H
#define ALGOS_H

/*
 * Array Sort
 */

void bubbleSortAsc(int *nums, int numSize);
void bubbleSortDesc(int *nums, int numSize);
void insertionSortAsc(int *nums, int numSize);
void insertionSortDesc(int *nums, int numSize);
void mergeSortAsc(int *nums, int start, int end);
void mergeSortDesc(int *nums, int start, int end);
void quickSortAsc(int * nums, int start, int end);
int partitionAsc(int *nums, int start, int end);
void quickSortDesc(int * nums, int start, int end);
int partitionDesc(int *nums, int start, int end);

/*
 * Array Search
 */

int binarySearch(int *nums, int numSize, int target);
int binarySearch2D(int nums[][10], int rows, int cols, int target);

/*
 * HashSet
 */

void duplicateNum(int *nums, int numSize);

/*
 * HashMap
 */

int* twoSum(int *nums, int numSize, int target, char *key, char *diffStr);

#endif
