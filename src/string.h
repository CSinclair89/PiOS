#ifndef STRING_H
#define STRING_H

#define CASE_DIFF 32

int strEqual(const char *str1, const char *str2);
void strCopy(char *dest, const char *src, int n);
void intToStr(int num, char *str, int size);
int strToInt(const char *str);
char *strCat(char *str1, char *str2);
void strReplace(char *src, char oldChar, char newChar);
void strToLower(char *src);
void strToUpper(char *src);
void arrayToString(int *nums, unsigned int len);
void array2DToString(int nums[3][10], unsigned int rows, unsigned int cols);

#endif
