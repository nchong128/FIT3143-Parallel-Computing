#include <stdio.h>
#include <stdlib.h>

int main() 
{
		
	int size = 0;
	int i = 0;
	printf("Specify the array size: ");
	scanf("%d", &size);
	
	//int array[size]; // Stack array
	int *array = (int*)malloc(size * sizeof(int)); // Heap (or dynamic) array
	printf("Size of the array (bytes): %ld\n", sizeof(array));

	char val = 'a'; // single byte stack variable
	printf("Size of the val (bytes): %ld\n", sizeof(val));

	char *pVal = &val;
	printf("Size of the pVal (bytes): %ld\n", sizeof(pVal));


	for(i = 0; i < size; i++){
		array[i] = i + 1;
	}

	
	int *y;
	int z;

	y = array;	// OK, because there's an implict conversion from int[] to int*
	//array = y;	// ERROR - if array variable is a stack array

	z = *array;	// Doesn't cause a crash
	z = *y;		// Crash if y doesn't point to anything

	printf("size of y = %ld\n", sizeof(y));
	printf("y = 0x%x\n", y);
	printf("z = %d\n", z);
	y++;		// OK, you can increment a pointer
	z = *y;	
	printf("New value of y = 0x%x\n", y);
	printf("New value of z = %d\n", z);
	
	//array++;	// ERROR, you can't do so to a stack array.
	
	free(array);
	return 0;
	
/*
sizeof(pointer) is always the same, regardless of the number of elements the pointer addresses, or the type of those elements. sizeof(array depends on both the size of the array, and the element type.
source: https://www.panix.com/~elflord/cpp/gotchas/index.shtml	

Aarays (Stack) vs Pointers:

1) the sizeof operator
o sizeof(array) returns the amount of memory used by all elements in array
o sizeof(pointer) only returns the amount of memory used by the pointer variable itself

2) the & operator
o &array is an alias for &array[0] and returns the address of the first element in array
o &pointer returns the address of pointer

3) a string literal initialization of a character array
o char array[] = “abc” sets the first four elements in array to ‘a’, ‘b’, ‘c’, and ‘\0’
o char *pointer = “abc” sets pointer to the address of the “abc” string (which may be stored in read-only memory and thus unchangeable)

4) Pointer variable can be assigned a value whereas array variable cannot be.

https://www.geeksforgeeks.org/pointer-vs-array-in-c/


*/
}


