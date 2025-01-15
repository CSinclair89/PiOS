#include "string.h"
#include "ds.h"
#include "io.h"

/*
 * Arrah Sort
 */

/*
 * Array Search
 */


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
