#include "string.h"
#include "ds.h"
#include "io.h"
#include "algos.h"

/*
 * Array Sort
 */

void bubbleSortAsc(int *nums, int numSize) {
	for (int i = 0; i < numSize; i++) {
		for (int j = 0; j < numSize - i - 1; j++) {
			if (nums[j] > nums[j + 1]) {
				int tmp = nums[j];
				nums[j] = nums[j + 1];
				nums[j + 1] = tmp;
			}
		}
	}
	return;
}

void bubbleSortDesc(int *nums, int numSize) {
	for (int i = 0; i < numSize; i++) {
		for (int j = 0; j < numSize - i - 1; j++) {
			if (nums[j] < nums[j + 1]) {
				int tmp = nums[j];
				nums[j] = nums[j + 1];
				nums[j + 1] = tmp;
			}
		}
	}
	return;
}

// Stable - Time complexity: worst case O(N^2), best case O(N)
void insertionSortAsc(int *nums, int numSize) {
	for (int i = 1; i < numSize; i++) {
		int j = i - 1;
		while (j >= 0 && nums[j + 1] < nums[j]) {
			int tmp = nums[j + 1];
			nums[j + 1] = nums[j];
			nums[j] = tmp;
			j--;
		}
	}
	return;			       
}

// Stable - Time complexity: worst case O(N^2), best case O(N)
void insertionSortDesc(int *nums, int numSize) {
	for (int i = 1; i < numSize; i++) {
		int j = i - 1;
		while (j >= 0 && nums[j + 1] > nums[j]) {
			int tmp = nums[j + 1];
			nums[j + 1] = nums[j];
			nums[j] = tmp;
			j--;
		}
	}
	return;			       
}

// Stable - Time complexity: worse case 0(nlogn)
void mergeSortAsc(int *nums, int start, int end) {
	if (end - start + 1 <= 0) return;
	

}


void mergeSortDesc(int *nums, int start, int end) {


}

// Time complexity - 
void quickSortAsc(int *nums, int start, int end){
	if (start >= end) return;
	int pivot = partitionAsc(nums, start, end);
	quickSortAsc(nums, start, pivot - 1);
	quickSortAsc(nums, pivot + 1, end);
}

int partitionAsc(int *nums, int start, int end) {
	int pivot = nums[end];
	int i = start - 1;
	for (int j = start; j <= end - 1; j++) {
		if (nums[j] < pivot) {
			i++;
			int tmp = nums[i];
			nums[i] = nums[j];
			nums[j] = tmp;
		}
	}
	i++;
	int tmp = nums[i];
	nums[i] = nums[end];
	nums[end] = tmp;
	return i;
}

void quickSortDesc(int *nums, int start, int end){
	if (start >= end) return;
	int pivot = partitionDesc(nums, start, end);
	quickSortDesc(nums, start, pivot - 1);
	quickSortDesc(nums, pivot + 1, end);
}

int partitionDesc(int *nums, int start, int end) {
	int pivot = nums[end];
	int i = start - 1;
	for (int j = start; j <= end - 1; j++) {
		if (nums[j] > pivot) {
			i++;
			int tmp = nums[i];
			nums[i] = nums[j];
			nums[j] = tmp;
		}
	}
	i++;
	int tmp = nums[i];
	nums[i] = nums[end];
	nums[end] = tmp;
	return i;
}


/*
 * Array Search
 */

int binarySearch(int *nums, int numSize, int target) {
	int l = 0, r = numSize;
	int isAscending = nums[0] < nums[numSize - 1];

	if (isAscending) printp("Array is sorted ascending.\n");
	else printp("Array is sorted descending.\n");
	
	while (l < r) {
		int m = (l + r) / 2;
		if (target == nums[m]) {
			printp("Found element %d at index %d!\n", target, m);
			return 1;
		}
		if (isAscending) {
			if (target > nums[m]) l = m + 1;
			else r = m;
		} else {
			if (target > nums[m]) r = m;
			else l = m + 1;
		}
	}
	printp("Did not find element %d in the array.\n", target);
	return 0;
}

int binarySearch2D(int nums[][10], int rows, int cols, int target) {

	int isAscending = nums[0][0] < nums[0][cols - 1];
        if (isAscending) printp("Array is sorted in ascending order.\n");
        else printp("Array is sorted in descending order.\n");

        for (int i = 0; i < cols; i++) {
                int l = 0, r = cols;

                while (l < r) {
                        int m = (l + r) / 2;
                        if (target == nums[i][m]) {
                                printp("Target (%d) found at index [%d][%d]\n", target, i, m);
                                return 1;
                        }
                        if (isAscending) {
                                if (target > nums[i][m]) l = m + 1;
                                else r = m;
                        } else {
                                if (target > nums[i][m]) r = m;
                                else l = m + 1;
                        }
                }
        }
	printp("Did not find element %d in the array.\n", target);
	return 0;
}
/*
 * HashSet
 */

void duplicateNum(int *nums, int numSize) {
	HashSet set;
	setInit(&set);
	for (int i = 0; i < numSize; i++) setAdd(&set, nums[i]);
}

/*
 * HashMap
 */

int* twoSum(int *nums, int numSize, int target, char *key, char *diffStr) {

	HashMap map;
	hashmapInit(&map);

        for (int i = 0; i < numSize; i++) {
                int diff = target - nums[i];

                // convert diff to a string
                intToStr(diff, diffStr, sizeof(diffStr));
                
                // check if diff exists as key in hashmap
                int index = hashmapGet(&map, diffStr);
                if(hashmapContainsKey(&map, diffStr) > 0) printp("{%d, %d}\n", index, i);
                
                // add current number to hash map
                intToStr(nums[i], key, sizeof(key));
                hashmapPut(&map, key, i);
	}
	return 0;
}
