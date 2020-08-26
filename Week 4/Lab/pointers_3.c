#include <stdio.h>
#include <stdlib.h>

int main() 
{
	int size = 0;
	int i;
	printf("Specify the array size: ");
	scanf("%d", &size);
	
	//int array[size];  // Stack array
	int *array = (int*)malloc(size * sizeof(int)); // Heap array
	
	for(i = 0; i < size; i++)
	{
		array[i] = i;
		printf("array[%d] = %d\n", i, array[i]);
	}
	
	free(array);
	return 0;

/*
The reason that stack array works at times is an implementation detail of how C is usually compiled. Typically, C memory is split into several regions, including the stack (for function calls and local variables) and the heap (for malloced objects). The stack typically has a much smaller size than the heap; usually it's something like 8MB. As a result, if you try to allocate a huge array with int array[n]; (e.g., n = 5,000,000),then you might exceed the stack's storage space, causing the segmentation fault. On the other hand, the heap usually has a huge size (say, as much space as is free on the system), and so mallocing a large object won't cause an out-of-memory error.

In general, be careful with variable-length arrays in C. They can easily exceed stack size. Prefer malloc unless you know the size is small or that you really only do want the array for a short period of time.	
Source: https://stackoverflow.com/questions/10575544/difference-between-array-type-and-array-allocated-with-malloc
*/	
	
	
}


