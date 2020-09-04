#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

struct valuestruct {
    int a;
    double b;
};

int main(int argc, char* argv[]) {
    int my_rank;
    int position = 0, count;
    struct valuestruct values;
    MPI_Datatype Valuetype;
    MPI_Datatype type[2] = { MPI_INT, MPI_DOUBLE };
    int blocklen[2] = { 1, 1 };
    MPI_Aint disp[2];
    char buff[100]; 

    
    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Get_address(&values.a, &disp[0]);
    MPI_Get_address(&values.b, &disp[1]);

    disp[1] = disp[1]-disp[0];
    disp[0] = 0;

    MPI_Type_create_struct(2, blocklen, disp, type, &Valuetype);
    MPI_Type_commit(&Valuetype);
    
    do {
        if (my_rank == 0) {
            printf("Enter a round number > 0 & a real number:\n");
            fflush(stdout);
            scanf("%d %lf", &values.a, &values.b);
            MPI_Pack(&values.a, 1, MPI_INT, buff, 100, &count, MPI_COMM_WORLD); 
            MPI_Pack(&values.b, 1, MPI_DOUBLE, buff, 100, &count, MPI_COMM_WORLD); 
        }
	    MPI_Bcast( &count, 1, MPI_INT, 0, MPI_COMM_WORLD ); 
	    MPI_Bcast( buff, count, MPI_PACKED, 0, MPI_COMM_WORLD );

	    MPI_Unpack( buff, count, &position, &values.a, 1, MPI_INT, MPI_COMM_WORLD );
	    MPI_Unpack( buff, count, &position, &values.b, 1, MPI_DOUBLE, MPI_COMM_WORLD );

        printf("Rank: %d. values.a = %d. values.b = %lf\n", my_rank, values.a, values.b);
        fflush(stdout);
    } while (values.a > 0);

    MPI_Type_free(&Valuetype);
    MPI_Finalize();
    return 0;
}