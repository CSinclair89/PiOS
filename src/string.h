#ifndef STRING_H
#define STRING_H

#define CASE_DIFF 32

int strEqual(const char *str1, const char *str2);
void strCopy(char *dest, const char *src, int n);
void intToStr(int num, char *str, int size);
int strToInt(const char *str);
void strReplace(char *src, char oldChar, char newChar);
void strToLower(char *src);
void strToUpper(char *src);

#endif
