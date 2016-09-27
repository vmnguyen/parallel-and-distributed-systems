#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	//define some variables to use later
	//rank is the process id
	//size is the total process run this program
	int rank, size;
	double a,b,s;

	MPI_Status status;
	// initilize the MPI part with 2 arguments is argc and argv
	MPI_Init(&argc,&argv);
	//get the rank of the processer and store it to rank
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	// get the size of the comm_world and store it to size
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	fprintf(stdout, "\n Process %d of %d processes starts \n", rank,size);
	if (rank == 1){
		// if rank equal to 1, set b equal 12.2;
		b = 12.2;
		// then recive a messege from 
		MPI_Recv(&a, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		s = b - a;
		fprintf(stdout, " Thwe result is: %f \n",s);
	}else
	{
		a = rank;
		MPI_Send(&a, 1, MPI_DOUBLE,1,11,MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}	


