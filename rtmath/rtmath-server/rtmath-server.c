//#include <memory>
//#include <iostream>
//#include <fstream>
//#include <string>
#include <mpi.h>
#include <stdio.h>

//#include "../rtmath/rtmath.h"

int main(int argc, char **argv)
{
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		int value = 17;
		int result = MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		if (result == MPI_SUCCESS)
			printf("Rank 0 OK!\n");
	} else if (rank == 1) {
		int value;
		int result = MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
			MPI_STATUS_IGNORE);
		if (result == MPI_SUCCESS && value == 17)
			printf("Rank 1 OK!\n");
	}
	MPI_Finalize();
	return 0;
}