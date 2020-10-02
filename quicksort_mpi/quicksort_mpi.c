#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#define SIZE 4

void printArray(int* arr, int arrSize);
int main(int argc, char** argv) {
    int numprocs, rank, rc;
    int* inputArr;
    MPI_Status recv_status;
    int inputArrSize;

    /* Initialise MPI env*/
    if ((rc = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
        printf("Error starting MPI program. Terminating\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("Process with rank: %d\n", rank);

    inputArrSize = atoi(argv[1]);
    inputArr = (int*) calloc(inputArrSize, sizeof(int));

    // root process
    if (rank == 0) {
        // load array with random numbers
        srand(time(NULL));
        for (int i = 0; i < inputArrSize; i++) {
            inputArr[i] = rand() % 100;
        }

        printArray(inputArr, inputArrSize);

        
    } else {

    }

    MPI_Finalize();
    return 0;
}

void printArray(int* arr, int arrSize) {
    // print arr
    for (int i = 0; i < arrSize; i++) {
        printf(" %d ", arr[i]);
    }
}