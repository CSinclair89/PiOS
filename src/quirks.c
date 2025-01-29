#include "quirks.h"
#include "io.h"

typedef struct Child {
	int val;
} Child;

typedef struct Parent {
	int val;
	Child child;
} Parent;

void structTest() {
	Parent parent1;
	parent1.val = 42;
	parent1.child.val = 23;

	printp("Value of parent1 (dot operator) is: %d\n", parent1.val);
	printp("Value of child1 (dot operator) is: %d\n", parent1.child.val);

	Parent parent_static;
	Parent *parent2 = &parent_static;
	parent2->val = 69;
	parent2->child.val = 73;

	printp("Value of parent2 (arrow operator) is: %d\n", parent2->val);
	printp("Value of child2 (arrow operator) is: %d\n", parent2->child.val);
}


void constTest(int val) {
	const int c = val;
        printp("Initial value of const int c: %d.\n", c);

        int *d = &c; // pointing at mem location of c
        *d = 40; // assigning a value to that memory location

        printp("New value of const int c: %d.\n", c);
	return;
}

void arrayAccess() {
	int arr[10] = {7, 3, 1, 5, 7, 3, 1, 2, 4, 9};
	printp("Normal array access at arr[7]: %d\n", arr[7]);
	printp("Weird array access at 7[arr]: %d\n", 7[arr]);
	printp("^ Fair to say that looks goofy as shit! ^\n");
}
