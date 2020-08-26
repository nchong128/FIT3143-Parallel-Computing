#include <stdio.h>

void arrProcess(int *ptr);

int main() 
{
	int x[5] = {1, 2, 3, 4, 5}; // stack
	int* ptr; // pointer - a variable that stores an address (memory address)


	
	// ptr is assigned the address of the third element
	ptr = &x[2]; 

	printf("*ptr = %d \n", *ptr);
	printf("*(ptr+1) = %d \n", *(ptr+1));
	printf("*(ptr-1) = %d\n", *(ptr-1));
	
	printf("ptr++ = 0x%x\n", ptr++);
	
	
	// Calling a function
	arrProcess(x);
	printf("x[0] = %d\n", x[0]);

	return 0;
}

void arrProcess(int *ptr) // pass by reference argument in Cs
{
	printf("*ptr = %d \n", *ptr);
	printf("*(ptr+1) = %d \n", *(ptr+1));
	printf("*(ptr-1) = %d\n", *(ptr-1));
	
	*ptr = 100;
	
	printf("ptr++ = 0x%x\n", ptr++);

	return;
}
