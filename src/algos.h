#ifndef ALGOS_H
#define ALGOS_H

/*
 * Array Sort
 */

void bubbleSort(int *nums, unsigned int len, char *pref);
void insertionSort(int *nums, unsigned int len, char *pref);
void deletionSort(int *nums, unsigned int len, char *pref);
void selectionSort(int *nums, unsigned int len, char *pref);
void quickSort(int *nums, int start, int end, char *pref);
void bogoSort(int *nums, unsigned int len, char *pref);


void mergeSortAsc(int *nums, int start, int end);
void mergeSortDesc(int *nums, int start, int end);

/*
 * Array Search
 */

int binarySearch(int *nums, unsigned int len, int target);
int binarySearch2D(int nums[][10], int rows, int cols, int target);

/*
 * HashSet
 */

void duplicateNum(int *nums, unsigned int len);

/*
 * HashMap
 */

int* twoSum(int *nums, unsigned int len, int target, char *key, char *diffStr);

#endif
