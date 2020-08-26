#include <stdio.h> 
// A normal function with an int parameter 
// and void return type 
void fun(int a) 
{ 
	printf("Value of a is %d\n", a); 
} 

int main() 
{ 
	// fun_ptr is a pointer to function fun() 
	void (*fun_ptr)(int) = &fun; // A function pointer stores the address of a function


	// Invoking fun() using fun_ptr 
	fun_ptr(10); 

	return 0; 
} 

