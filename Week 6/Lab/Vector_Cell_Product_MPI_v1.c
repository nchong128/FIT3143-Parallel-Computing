////////////////////////////////////////////////////////////////////////////
// Vector_Cell_Product_MPI_v1.c
// -------------------------------------------------------------------------
//
// Performs a cell by cell product between two vectors using MPI
// Not optimized: Entire data being sent to all processes
//
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

// Function prototype
int* ReadFromFile(char *pFilename, int *pOutRow, int *pOutCol);
void WriteToFile(char *pFilename, int *pMatrix, int inRow, int inCol);

int main()
{
	int row1, col1, row2, col2;
	int i, j;
	int my_rank;
	int p;
	int *pArrayNum1 = NULL;
	int *pArrayNum2 = NULL;
	int *pArrayNum3 = NULL;
	struct timespec start, end, startComm, endComm; 
	double time_taken; 

	MPI_Status status;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); // You can also MPI_Wtime()

	if(my_rank == 0)
	{
		// STEP 1: Only the root process reads VA.txt and VB.txt into its own memory
		printf("Rank: %d. MPI Implementation version 1. Commence Reading\n", my_rank);

	    	// Call the read from file function
		pArrayNum1 = ReadFromFile("VA.txt", &row1, &col1);
		
	    	if(pArrayNum1 == 0)
		{
			printf("Rank: %d. Read failed.\n", my_rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			return 0;
		}
		

	    	// Call the read from file function
		pArrayNum2 = ReadFromFile("VB.txt", &row2, &col2);
		
	    	if(pArrayNum2 == 0)
		{
			printf("Rank: %d. Read failed.\n", my_rank);
			free(pArrayNum1);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			return 0;
		}
		
		if(row1 != row2 || col1 != col2)
		{
		    printf("Rank: %d. Not matching row and column values between the arrays.\n", my_rank);
		    free(pArrayNum1);
		    free(pArrayNum2);
		    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		    return 0;
		}
		printf("Rank: %d. Read complete\n", my_rank);
		
	}

	//printf("Rank: %d. row: %d. Col: %d\n", my_rank, row1, col1);
	// STEP 2: Broadcast the arrays to all other MPI processess in the group	
	MPI_Bcast(&row1, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&row2, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(my_rank != 0){
		pArrayNum1 = (int*)malloc(row1 * sizeof(int)); // We are performing vector cell product, so col value is by default 1
		pArrayNum2 = (int*)malloc(row2 * sizeof(int)); // We are performing vector cell product, so col value is by default 1
	}
	
	
	clock_gettime(CLOCK_MONOTONIC, &startComm); 
	MPI_Bcast(pArrayNum1, row1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(pArrayNum2, row2, MPI_INT, 0, MPI_COMM_WORLD);
	clock_gettime(CLOCK_MONOTONIC, &endComm); 
	time_taken = (endComm.tv_sec - startComm.tv_sec) * 1e9; 
    	time_taken = (time_taken + (endComm.tv_nsec - startComm.tv_nsec)) * 1e-9; 
	printf("Rank: %d. Comm time (s): %lf\n\n", my_rank, time_taken);
	
	// Perform the cell by cell product
	printf("Rank: %d. Compute\n", my_rank);
	
	pArrayNum3 = (int*)malloc(row1 * sizeof(int)); // Can use row2 as an alternative
	
	// Basic workload distribution among MPI processes 
	// Row based partitioning or row segmentation
	int elementsPerProcess = row1 / p;
	int elementsPerProcessRemain = row1 % p;
	
	int startPoint = my_rank * elementsPerProcess;
	int endPoint  = startPoint + elementsPerProcess;
	if(my_rank == p-1)
	{
		// Last node, factor in the remainder
		endPoint += elementsPerProcessRemain;
	}
	
	// STEP 3 - Parallel computing takes place here
	for(i = startPoint; i< endPoint; i++){
		// The second loop is intentionally included to increase the computational time
		for(j = 0; j< 500; j++){
	    		pArrayNum3[i] = pArrayNum1[i] * pArrayNum2[i];
		}
	}

	// STEP 4 - Send the arrays results back to the root process
	if(my_rank == 0)
	{
		// Initialize the offset based on Rank 0's workload
		int offset = elementsPerProcess;
		for(i = 1; i < p; i++){
			if(i != p-1)
				MPI_Recv((int*)pArrayNum3 + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			else
				MPI_Recv((int*)pArrayNum3 + offset, (elementsPerProcess + elementsPerProcessRemain), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			offset += elementsPerProcess;
		}
		// STEP 5: Write to file
		printf("Rank: %d. Commence Writing\n", my_rank);
		WriteToFile("VC.txt", pArrayNum3, row1, col1);
		printf("Rank: %d. Write complete\n", my_rank);
	}else{
		// Other MPI processes
		MPI_Send((int*)pArrayNum3 + startPoint, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	free(pArrayNum1);
	free(pArrayNum2);
	free(pArrayNum3);
	
	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    	time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
	printf("Rank: %d. Overall time (s): %lf\n\n", my_rank, time_taken); // tp
	
	MPI_Finalize();
	return 0;
}

// Function definition
int* ReadFromFile(char *pFilename, int *pOutRow, int *pOutCol)
{
	int i, j;
	int row, col;
	FILE *pFile = fopen(pFilename, "r");
	if(pFile == NULL)
	{
		printf("Error: Cannot open file\n");
		return 0;
	}

	fscanf(pFile, "%d%d", &row, &col);
	int *pMatrix = (int*)malloc(row * col * sizeof(int)); // Heap array

	// Reading a 2D matrix into a 1D heap array
	for(i = 0; i < row; i++){
	    for(j = 0; j < col; j++){
		fscanf(pFile, "%d", &pMatrix[(i * col) + j]);
		}
	}
	fclose(pFile);

	*pOutRow = row; // Dereferencing the pointer
	*pOutCol = col; // Dereferencing the pointer
	return pMatrix;
}

void WriteToFile(char *pFilename, int *pMatrix, int inRow, int inCol)
{
	int i, j;
	FILE *pFile = fopen(pFilename, "w");
	fprintf(pFile, "%d\t%d\n", inRow, inCol);
	for(i = 0; i < inRow; i++){
	    for(j = 0; j < inCol; j++){
		fprintf(pFile, "%d\t", pMatrix[(i * inCol) + j]);
		}
		fprintf(pFile, "\n");
	}
	fclose(pFile);
}
