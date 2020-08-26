#include <stdio.h>
#include <stdlib.h>

int* createArray(int arrSize);


int main() 
{
	int size = 0;
	int i;
	int *pArr = NULL;
	printf("Specify the array size: ");
	scanf("%d", &size);
	
	// Calling a function
	pArr = createArray(size);
	
	for(i = 0; i < size; i++)
		printf("array[%d] = %d\n", i, pArr[i]);

	free(pArr);
	return 0;
}

int* createArray(int arrSize) // pass by value argument
{
	int i;
	//int array[arrSize]; // declaring a stack array
	int *pArray = (int*)malloc(arrSize * sizeof(int)); // declaring a heap array
	for(i = 0; i < arrSize; i++)
		pArray[i] = i;

	return pArray;
	
	/*
	With stack based arrays you are declaring an object with automatic storage duration. 
	This means that the array lives only as long as the function that calls it exists. 
	With heap based arrays, you are getting memory with dynamic storage duration, 
	which means that it will exist until it is explicitly deallocated with free.
	Source: https://stackoverflow.com/questions/10575544/difference-between-array-type-and-array-allocated-with-malloc
	*/
}
