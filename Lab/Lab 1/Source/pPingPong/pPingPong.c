#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char ** argv)
{
	int mod, rank, size;
	double a,b,c;
	a = 1;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_rank(MPI_COMM_WORLD, &size);


	printf ("Rank %d is running \n", rank);
	// if rank = 0
	if (rank%2 == 0)
	{
		a = rank;
		MPI_Send(&a, 1, MPI_INT, rank +1, 11, MPI_COMM_WORLD);
		printf("****Send %d to process %d **** \n", rank, rank+1);
	}else{
		MPI_Recv(&a, 1, MPI_INT, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("Recivice %d from process %d and send %d to process %d \n",rank-1,rank-1, rank, rank-1);
		a = rank;
		MPI_Send(&a, 1, MPI_INT, rank-1, 11, MPI_COMM_WORLD );
	}
	
	MPI_Finalize();
	return 0;	

}
