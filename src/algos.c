#include "string.h"
#include "ds.h"
#include "io.h"
#include "algos.h"
#include "rand.h"

/*
 * Array Sort
 */

void bubbleSort(int *nums, unsigned int len, char *pref) {
	
	if (strEqual(pref, "asc")) goto asc;
	else if (strEqual(pref, "desc")) goto desc;
	else {
		printp("Invalid input argument.\n");
		return;
	}
	
	asc:
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len - i - 1; j++) {
			if (nums[j] > nums[j + 1]) {
				int tmp = nums[j];
				nums[j] = nums[j + 1];
				nums[j + 1] = tmp;
			}
		}
	}
	goto exit;

	desc:
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len - i - 1; j++) {
			if (nums[j] < nums[j + 1]) {
				int tmp = nums[j];
				nums[j] = nums[j + 1];
				nums[j + 1] = tmp;
			}
		}
	}

	exit:
	return;
}

// Stable - Time complexity: worst case O(N^2), best case O(N)
void insertionSort(int *nums, unsigned int len, char *pref) {
	
	if (strEqual(pref, "asc")) goto asc;
	else if (strEqual(pref, "desc")) goto desc;
	else {
		printp("Invalid input argument.\n");
		return;
	}

	asc:
	for (int i = 1; i < len; i++) {
		int j = i - 1;
		while (j >= 0 && nums[j + 1] < nums[j]) {
			int tmp = nums[j + 1];
			nums[j + 1] = nums[j];
			nums[j] = tmp;
			j--;
		}
	}
	goto exit;

	desc:
	for (int i = 1; i < len; i++) {
		int j = i - 1;
		while (j >= 0 && nums[j + 1] > nums[j]) {
			int tmp = nums[j + 1];
			nums[j + 1] = nums[j];
			nums[j] = tmp;
			j--;
		}
	}
	
	exit:
	return;
}

void quickSort(int *nums, int start, int end, char *pref) {

	// Logic Header	
	if (strEqual(pref, "asc")) goto asc;
	else if (strEqual(pref, "desc")) goto desc;
	else {
		printp("Invalid input argument.\n");
		return;
	}

	asc:
	if (end - start + 1 <= 1) return;

	int pivotAsc = nums[end];
	int leftAsc = start;

	for (int i = start; i < end; i++) {
	       if (nums[i] < pivotAsc) {
		       int tmp = nums[leftAsc];
		       nums[leftAsc] = nums[i];
		       nums[i] = tmp;
		       leftAsc++;
	       }
	}
	nums[end] = nums[leftAsc];
	nums[leftAsc] = pivotAsc;

	quickSort(nums, start, leftAsc - 1, "asc");
	quickSort(nums, leftAsc + 1, end, "asc");
	goto exit;

	desc:
	if (end - start + 1 <= 1) return;

	int pivotDesc = nums[end];
	int leftDesc = start;

	for (int i = start; i < end; i++) {
	       if (nums[i] > pivotDesc) {
		       int tmp = nums[leftDesc];
		       nums[leftDesc] = nums[i];
		       nums[i] = tmp;
		       leftDesc++;
	       }
	}
	nums[end] = nums[leftDesc];
	nums[leftDesc] = pivotDesc;

	quickSort(nums, start, leftDesc - 1, "desc");
	quickSort(nums, leftDesc + 1, end, "desc");

	exit:
	return;
}

void selectionSort(int *nums, unsigned int len, char *pref) {
	
	if (strEqual(pref, "asc")) goto asc;
	else if (strEqual(pref, "desc")) goto desc;
	else {
		printp("Invalid input argument.\n");
		return;
	}

	asc:
	for (int i = 0; i < len - 1; i++) {
		int min = i;
		for (int j = i + 1; j < len; j++) {
			if (nums[min] > nums[j]) min = j;
		}
		int tmp = nums[i];
		nums[i] = nums[min];
		nums[min] = tmp;
	}
	goto exit;

	desc:
	for (int i = 0; i < len - 1; i++) {
		int max = i;
		for (int j = i + 1; j < len; j++) {
			if (nums[max] < nums[j]) max = j;
		}
		int tmp = nums[i];
		nums[i] = nums[max];
		nums[max] = tmp;
	}

	exit:
	return;
}

void bogoSort(int *nums, unsigned int len, char *pref) {

	head:
	if (strEqual(pref, "asc")) goto asc;
	else if (strEqual(pref, "desc")) goto desc;
	else {
		printp("Invalid input argument.\n");
		goto exit;
	}

	asc:
	for (unsigned int i = 1; i < len; i++) {
		if (nums[i] < nums[i - 1]) goto shuffle;
	}
	goto exit;

	desc:
	for (unsigned int i = 1; i < len; i++) {
		if (nums[i] > nums[i - 1]) goto shuffle;
	}
	goto exit;

	// Fisher-Yates Shuffle
	shuffle:
	for (unsigned int i = 0; i < len; i++) {
		int j = randRange(0, len + 1);
		int tmp = nums[i];
		nums[i] = nums[j];
		nums[j] = tmp;
	}
	goto head;

	exit:
	return;
}



// Stable - Time complexity: worst case 0(nlogn)
void mergeSortAsc(int *nums, int start, int end) {
	if (end - start + 1 <= 0) return;
	

}


void mergeSortDesc(int *nums, int start, int end) {


}
/*
 * Array Search
 */

int binarySearch(int *nums, unsigned int len, int target) {
	int l = 0, r = len;
	int isAscending = nums[0] < nums[len - 1];

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

void duplicateNum(int *nums, unsigned int len) {
	HashSet set;
	setInit(&set);
	for (int i = 0; i < len; i++) setAdd(&set, nums[i]);
}

/*
 * HashMap
 */

int* twoSum(int *nums, unsigned int len, int target, char *key, char *diffStr) {

	HashMap map;
	hashmapInit(&map);

        for (int i = 0; i < len; i++) {
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
