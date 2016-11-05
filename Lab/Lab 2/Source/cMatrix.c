<<<<<<< HEAD
/*
 *  mmult.c: matrix multiplication using MPI.
 * There are some simplifications here. The main one is that matrices B and C
 * are fully allocated everywhere, even though only a portion of them is
 * used by each processor (except for processor 0)
 */

#include <mpi.h>
#include <stdio.h>

#define SIZE 8			/* Size of matrices */

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

void fill_matrix(int m[SIZE][SIZE])
{
	static int n = 0;
	int i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			m[i][j] = n++;
}

void print_matrix(int m[SIZE][SIZE])
{
	int i, j = 0;
	for (i = 0; i < SIZE; i++) {
		printf("\n\t| ");
		for (j = 0; j < SIZE; j++)
			printf("%2d ", m[i][j]);
		printf("|");
	}
}


int main(int argc, char *argv[])
{
	int myrank, P, from, to, i, j, k;
	int tag = 666;		/* any value will do */
	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	/* who am i */
	MPI_Comm_size(MPI_COMM_WORLD, &P); /* number of processors */

	/* Just to use the simple variants of MPI_Gather and MPI_Scatter we */
	/* impose that SIZE is divisible by P. By using the vector versions, */
	/* (MPI_Gatherv and MPI_Scatterv) it is easy to drop this restriction. */

	if (SIZE % P != 0) {
		if (myrank == 0) printf("Matrix size not divisible by number of processors\n");
		MPI_Finalize();
		exit(-1);
	}

	from = myrank * SIZE / P;
	to = (myrank + 1) * SIZE / P;

	/* Process 0 fills the input matrices and broadcasts them to the rest */
	/* (actually, only the relevant stripe of A is sent to each process) */

	if (myrank == 0) {
		fill_matrix(A);
		fill_matrix(B);
	}

	MPI_Bcast (B, SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter (A, SIZE * SIZE / P, MPI_INT, A[from], SIZE * SIZE / P, MPI_INT, 0, MPI_COMM_WORLD);

	printf("computing slice %d (from row %d to %d)\n", myrank, from, to - 1);
	for (i = from; i < to; i++)
		for (j = 0; j < SIZE; j++) {
			C[i][j] = 0;
			for (k = 0; k < SIZE; k++)
				C[i][j] += A[i][k] * B[k][j];
		}

	MPI_Gather (C[from], SIZE * SIZE / P, MPI_INT, C, SIZE * SIZE / P, MPI_INT, 0, MPI_COMM_WORLD);

	if (myrank == 0) {
		printf("\n\n");
		print_matrix(A);
		printf("\n\n\t       * \n");
		print_matrix(B);
		printf("\n\n\t       = \n");
		print_matrix(C);
		printf("\n\n");
	}

	MPI_Finalize();
	return 0;
}



=======
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define NRA 10
#define NCA 5
#define NCB 3
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

MPI_Status status;

int main(int argc, char **argv) {

	double A[NRA][NCA],
	       B[NCA][NCB],
	       C[NRA][NCB];				/* the overall vector sum */
	int		numProcs,			/* number of processes in virtual machine */
	        rank,				/* our rank */
	        i,	j,				/* simple counter */
	        count,
	        from, to,k;				/* the number of elements in a partition */



	/******** master and slaves *********/

	// initialize MPI, etc.
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	// compute partition size
	count = NRA / numProcs;


	/********* master process ***********/
	if (rank == MASTER) {
		// Initialize the vector
		for (i = 0; i < NRA; i++)
			for (j = 0; j < NCA; j++)
				A[i][j] = i + j + 1;
		for (i = 0; i < NCA; i++)
			for (j = 0; j < NCB; j++)
				B[i][j] = i * j + 1;

		// check that numProcs divides NELTS else abort
		if (count * numProcs != NRA) {
			printf("Number of processes (%d) must divide evenly into problem size (%d).\n", numProcs, NRA);
			fflush(stdout);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
	} // end master


	/******** master and slaves *********/
	// distribute vector partitions among processes using MPI_Gather
	from = rank * NRA / numProcs;
	to = (rank + 1) * NRA / numProcs;

	MPI_Bcast (B, NCA * NCB, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter (A, NRA * NCA / numProcs, MPI_INT, A[from], NRA * NCA / numProcs, MPI_INT, 0, MPI_COMM_WORLD);
	
	// compute our partial sum (MASTER AND SLAVES)
	/*
	for (i = from; i < to; i++)
		for (j = 0; j < NCA; j++) {
			C[i][j] = 0;
			for (k = 0; k < NCB; k++){
				C[i][j] += A[i][k] * B[k][j];
				printf("Counting i = %d, j = %d, k = %d. \n", i,j,k);
			}

		}
	*/
	for (k = 0; k < NCB; k++){
			printf("Counting from = %d, to = %d. \n", from,to);
			for (i = from; i < to; i++) {

				C[i][k] = 0.0;
				for (j = 0; j < NCA; j++){
					C[i][k] = C[i][k] + A[i][j] * B[j][k];
					printf("Counting i = %d, j = %d, k = %d. \n", i,j,k);
				}

			}
		}
	// gather partial sums from processes using MPI_Gather
	MPI_Gather (C[from], NRA * NCA / numProcs, MPI_INT, C, NRA * NCA / numProcs, MPI_INT, 0, MPI_COMM_WORLD);


	/********* master process ***********/
	if (rank == MASTER) {
		// sum partial results and print result
		for (int m = 0; m < NRA; m++) {
			for (int n = 0; n < NCB; n++) {
				printf(" %lf ", C[m][n]);

			}
			printf("\n");
		}
	}  // end master


	/******** master and slaves *********/
	MPI_Finalize();
	return 0;
}  // main
>>>>>>> 5acf519ccad01e77ca8e11dbda65e62d99508136
