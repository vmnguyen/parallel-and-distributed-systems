#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv){
	int rank, size;
	double a;
	double b;
	double s;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	fprintf(stdout, "\n Process %d of %d processes starts \n", rank, size);
	if (rank == 1) {
		b = 12.2;
		MPI_Recv(&a,1,MPI_DOUBLE, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,
		&status);
		s = b - a;
		fprintf(stdout, " The result is : %f \n", s);
	}
	else {
		a = rank;
		MPI_Send(&a,1, MPI_DOUBLE, 1, 11, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 68;
}
