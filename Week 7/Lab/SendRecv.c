////////////////////////////////////////////////////////////////////////////
// SendRecv.c
// -------------------------------------------------------------------------
//
// Implements a send and receive of buffer between two processes
// Demonstrates the use of blocking and non-blocking MPI communication
//
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

// Function prototype
int* ReadFromFile(char *pFilename, int *pOutRow, int *pOutCol);
void WriteToFile(char *pFilename, int *pMatrix, int inRow, int inCol);

int main()
{
	int row, col;
	int rowsDetached;
	int my_rank;
	int p;
	int *pArrayNum1 = NULL;
	int *pArrayNum2 = NULL;
	int *pArrayNum3 = NULL;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	MPI_Status status[2];
	MPI_Request req[2];

	if(my_rank == 0){
		printf("Rank: %d. Reading VA text file\n", my_rank);

	    	// Call the read from file function
		pArrayNum1 = ReadFromFile("VA.txt", &row, &col);
		
	    	if(pArrayNum1 == 0)
		{
			printf("Rank: %d. Read failed.\n", my_rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			return 0;
		}

		printf("Rank: %d. Read complete. Rows read: %d\n", my_rank, row);
		/*
		for(int i = 0; i < row; i++)
			printf("Rank: %d. pArrayNum1[%d]: %d\n", my_rank, i, pArrayNum1[i]);
		*/	
		pArrayNum2 = (int*)malloc(row * sizeof(int));
		
		/* MPI_Ssend and MPI_Recv */
		/*
		MPI_Ssend(pArrayNum1, row, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(pArrayNum2, row, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		*/
		
		/* MPI_Send and MPI_Recv */
		/*
		MPI_Send(pArrayNum1, row, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(pArrayNum2, row, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		*/

		/* MPI_Bsend and MPI_Recv */
		/*
		pArrayNum3 = (int*)malloc(row * sizeof(int)); // Behaves as a buffer
		MPI_Buffer_attach(pArrayNum3, (row * sizeof(int)) + MPI_BSEND_OVERHEAD);
		MPI_Bsend(pArrayNum1, row, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(pArrayNum2, row, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Buffer_detach(&pArrayNum3[0], &rowsDetached);
		*/
		
		/* MPI_Isend and MPI_Irecv */
		/*
		MPI_Isend(pArrayNum1, row, MPI_INT, 1, 0, MPI_COMM_WORLD, &req[0]);
		//MPI_Wait(&req, &status);
		MPI_Irecv(pArrayNum2, row, MPI_INT, 1, 0, MPI_COMM_WORLD, &req[1]);
		//MPI_Wait(&req, &status);	
		MPI_Waitall(2, req, status); // Blocks
		*/
	
		/* MPI_Sendrecv*/
		
		MPI_Sendrecv(pArrayNum1, row, MPI_INT, 1, 0, pArrayNum2, row, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Blocking function
		
		
		printf("Rank: %d. Send and receive operations are complete\n", my_rank);
		
		WriteToFile("VB_by_Rank_0.txt", pArrayNum2, row, col);
		printf("Rank: %d. Write complete. Rows written: %d\n", my_rank, row);
		
		
	}else if(my_rank == 1){
		printf("Rank: %d. Reading VB text file\n", my_rank);
		
		// Call the read from file function
		pArrayNum2 = ReadFromFile("VB.txt", &row, &col);
		
	    	if(pArrayNum2 == 0){
			printf("Rank: %d. Read failed.\n", my_rank);
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			return 0;
		}
		
		printf("Rank: %d. Read complete. Rows read: %d\n", my_rank, row);
		/*
		for(int i = 0; i < row; i++)
			printf("Rank: %d. pArrayNum2[%d]: %d\n", my_rank, i, pArrayNum2[i]);
		*/
		pArrayNum1 = (int*)malloc(row * sizeof(int));
		
		/* MPI_Ssend and MPI_Recv */
		/*
		MPI_Recv(pArrayNum1, row, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Ssend(pArrayNum2, row, MPI_INT, 0, 0, MPI_COMM_WORLD);
		*/
		
		/* MPI_Send and MPI_Recv */
		/*
		MPI_Recv(pArrayNum1, row, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(pArrayNum2, row, MPI_INT, 0, 0, MPI_COMM_WORLD);
		*/
		/* MPI_Bsend and MPI_Recv */
		/*
		pArrayNum3 = (int*)malloc(row * sizeof(int));
		MPI_Buffer_attach(pArrayNum3, (row * sizeof(int)) + MPI_BSEND_OVERHEAD);
		MPI_Bsend(pArrayNum2, row, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Recv(pArrayNum1, row, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Buffer_detach(&pArrayNum3[0], &rowsDetached);
		*/
		/* MPI_Isend and MPI_Irecv */
		/*
		MPI_Isend(pArrayNum2, row, MPI_INT, 0, 0, MPI_COMM_WORLD, &req[0]);
		//MPI_Wait(&req, &status);
		MPI_Irecv(pArrayNum1, row, MPI_INT, 0, 0, MPI_COMM_WORLD, &req[1]);
		//MPI_Wait(&req, &status);
		MPI_Waitall(2, req, status); // Block until the data has been sent and the data has been received
		*/
		
		/* MPI_Sendrecv*/
		
		MPI_Sendrecv(pArrayNum2, row, MPI_INT, 0, 0, pArrayNum1, row, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Blocking function
		

		printf("Rank: %d. Send and receive operations are complete\n", my_rank);
		
		WriteToFile("VA_by_Rank_1.txt", pArrayNum1, row, col);
		printf("Rank: %d. Write complete. Rows written: %d\n", my_rank, row);
		
	}else{
		printf("Rank: %d. No tasks to do. This programs only requires two MPI processes.\n", my_rank);
		MPI_Finalize();
		return 0;
	}

	
	free(pArrayNum1);
	free(pArrayNum2);
	//free(pArrayNum3);
	
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
