////////////////////////////////////////////////////////////////////////////
// Vector_Cell_Product_MPI_v3.c
// -------------------------------------------------------------------------
//
// Performs a cel by cell product between two vectors using MPI
// Optimized to reduce size of data being transferred between processes
// Use non blocking communication (MPI Isend and MPI Irecv)
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
	int offset;
	struct timespec start, end, startComm, endComm; 
	double time_taken; 

	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	MPI_Status status[p-1];
	MPI_Request req[p-1];

    	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); 

	if(my_rank == 0)
	{
		// STEP 1: Only the root process reads VA.txt and VB.txt into its own memory
		printf("Rank: %d. MPI Implementation version 3. Commence Reading\n", my_rank);

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

	// Broadcast the arrays to all other MPI processess in the group	
	MPI_Bcast(&row1, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&row2, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	// Basic workload distribution among MPI processes 
	// Row based partitioning or row segmentation
	int elementsPerProcess = row1 / p;
	int elementsPerProcessRemain = row1 % p;
	
	int startPoint = my_rank * elementsPerProcess;
	int endPoint  = startPoint + elementsPerProcess;
	if(my_rank == p-1){
		// Last node, factor in the remainder
		endPoint += elementsPerProcessRemain;
	}

	// STEP 2: Send relevant portions the arrays to all other MPI processess in the group	
	clock_gettime(CLOCK_MONOTONIC, &startComm); 
	if(my_rank == 0){
		pArrayNum3 = (int*)malloc(row1 * sizeof(int)); // Can use row2 as an alternative
		offset = elementsPerProcess;
		for(i = 1; i < p; i++){
			
			if(i != p-1){
				MPI_Isend((int*)pArrayNum1 + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);
				MPI_Isend((int*)pArrayNum2 + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);
				
			}else{				
				MPI_Isend((int*)pArrayNum1 + offset, (elementsPerProcess + elementsPerProcessRemain), MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);
				MPI_Isend((int*)pArrayNum2 + offset, (elementsPerProcess + elementsPerProcessRemain), MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);	
			}
			offset += elementsPerProcess;
		}
	}else{
		pArrayNum1 = (int*)malloc((endPoint-startPoint) * sizeof(int));
		pArrayNum2 = (int*)malloc((endPoint-startPoint) * sizeof(int)); 
		pArrayNum3 = (int*)malloc((endPoint-startPoint) * sizeof(int));
		
		//MPI_Recv(pArrayNum1, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//MPI_Recv(pArrayNum2, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Irecv(pArrayNum1, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(pArrayNum2, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD, &req[1]);
		MPI_Waitall(2, req, status);	
	}
	clock_gettime(CLOCK_MONOTONIC, &endComm); 
	time_taken = (endComm.tv_sec - startComm.tv_sec) * 1e9; 
    	time_taken = (time_taken + (endComm.tv_nsec - startComm.tv_nsec)) * 1e-9; 
	printf("Rank: %d. Comm time (s): %lf\n\n", my_rank, time_taken);
	
	/* // TEST CODE to check if the arrays were correctly distributed to all MPI processes 
	char name[32] = {0};
	sprintf(name, "file_%d.txt", my_rank);
	FILE *pFileS = fopen(name, "w");
	for(i = 0; i < ((my_rank == p-1) ? (elementsPerProcess + elementsPerProcessRemain) : elementsPerProcess); i++){
		fprintf(pFileS, "%d\t%d\n", pArrayNum1[i], pArrayNum2[i]);
	}
	fclose(pFileS);
	*/

	// STEP 3 - Parallel computing takes place here
	printf("Rank: %d. Compute\n", my_rank);

	for(i = 0; i<  ((my_rank == p-1) ? (elementsPerProcess + elementsPerProcessRemain) : elementsPerProcess); i++){
		// The second loop is intentionally included to increase the computational time
		for(j = 0; j< 500; j++){
	    		pArrayNum3[i] = pArrayNum1[i] * pArrayNum2[i];
		}
	}
	
	
	// Wait for the previous non blocking sends to complete (on root rank)
	if(my_rank == 0){
		MPI_Waitall(p-1, req, status);
	}
	

	// STEP 4 - Send the arrays results back to the root process
	if(my_rank == 0)
	{
		// Initialize the offset based on Rank 0's workload
		offset = elementsPerProcess;
		
		for(i = 1; i < p; i++){
			if(i != p-1){
				//MPI_Recv((int*)pArrayNum3 + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Irecv((int*)pArrayNum3 + offset, elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);
			}else{
				//MPI_Recv((int*)pArrayNum3 + offset, (elementsPerProcess + elementsPerProcessRemain), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Irecv((int*)pArrayNum3 + offset, (elementsPerProcess + elementsPerProcessRemain), MPI_INT, i, 0, MPI_COMM_WORLD, &req[i-1]);
			}
			offset += elementsPerProcess;
		}
		MPI_Waitall(p-1, req, status);

		// STEP 5: Write to file
		printf("Rank: %d. Commence Writing\n", my_rank);
		WriteToFile("VC.txt", pArrayNum3, row1, col1);
		printf("Rank: %d. Write complete\n", my_rank);
	}else{
		// MPI_Send((int*)pArrayNum3, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Isend((int*)pArrayNum3, (endPoint - startPoint), MPI_INT, 0, 0, MPI_COMM_WORLD, &req[0]);
		MPI_Wait(&req[0], &status[0]);
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
