ALL: Serial POSIX OpenMP MPI_v1 MPI_v2

Serial: Vector_Cell_Product.c
	gcc -Wall Vector_Cell_Product.c -o serialOut

POSIX: Vector_Cell_Product_Parallel_POSIX.c
	gcc -Wall Vector_Cell_Product_Parallel_POSIX.c -o posixOut -lpthread

OpenMP: Vector_Cell_Product_OMP.c
	gcc -fopenmp -Wall Vector_Cell_Product_OMP.c -o openmpOut
	
MPI_v1: Vector_Cell_Product_MPI_v1.c
	mpicc -Wall Vector_Cell_Product_MPI_v1.c -o mpiOutv1
	
MPI_v2: Vector_Cell_Product_MPI_v2.c
	mpicc -Wall Vector_Cell_Product_MPI_v2.c -o mpiOutv2

run:
	./serialOut
	./posixOut
	export OMP_NUM_THREADS=6
	./openmpOut
	mpirun -np 6 mpiOutv1
	mpirun -np 6 mpiOutv2

clean :
	/bin/rm -f serialOut *.o
	/bin/rm -f posixOut *.o
	/bin/rm -f openmpOut *.o
	/bin/rm -f mpiOutv1 *.o
	/bin/rm -f mpiOutv2 *.o
	/bin/rm -f VC.txt
