#ifndef ALGOS_H
#define ALGOS_H

/*
 * Array Sort
 */

void bubbleSort(int *nums, int numSize, char *pref);
void insertionSort(int *nums, int numSize, char *pref);
void deletionSort(int *nums, int numSize, char *pref);
void selectionSort(int *nums, int numSize, char *pref);
void quickSort(int *nums, int start, int end, char *pref);

void bubbleSort2(int *nums, int numSize, char *pref);
void insertionSort2(int *nums, int numSize, char *pref);
void quickSort2(int *nums, int numSize, char *pref);

void mergeSortAsc(int *nums, int start, int end);
void mergeSortDesc(int *nums, int start, int end);

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
