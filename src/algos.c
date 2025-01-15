#include "string.h"
#include "ds.h"
#include "io.h"

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

// Time complexity: worst case O(N^2), best case O(N)
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

// Time complexity: worst case O(N^2), best case O(N)
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
