#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <mpi.h>
#include "mergeTool.h"

#define DYN_WIDTH 100
#define DYN_HEIGHT 100
#define STA_WIDTH 100
#define STA_HEIGHT 100

#define MASTER 0

void execution(){
	MPI_Barrier(MPI_COMM_WORLD);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == MASTER){
		static bool isStarted = false;
		static time_t begin;
		static time_t end;
		if (isStarted == true){
			end = clock();
			double difference = (double) (end - begin) / CLOCKS_PER_SEC;
			printf("execution time: %f\n", difference);
			isStarted = false;
		} else {
			begin = clock();
			isStarted = true;
		}
	}
}

int main(int argc, char **argv)
{
	int rank, worldRank;
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldRank);

	execution();

	struct matrix *dynamicMatrix = NULL;
	struct matrix *staticMatrix = NULL;
	if (rank == MASTER){
		srand(time(NULL));

		dynamicMatrix = createMatrix(DYN_WIDTH, DYN_HEIGHT);
		fillMatrix(dynamicMatrix);
		printMatrix(dynamicMatrix); 

		staticMatrix = createMatrix(STA_WIDTH, STA_HEIGHT);
		fillMatrix(staticMatrix);
		printMatrix(staticMatrix);
	}

	MPI_Barrier(MPI_COMM_WORLD);
    struct mergeSpace *bestMerge = (struct mergeSpace *)getBestMergeSolution(dynamicMatrix, staticMatrix);
    MPI_Barrier(MPI_COMM_WORLD);
    struct matrix *merged = NULL;
    if (rank == MASTER) {
    	merged = merge(bestMerge);
    	printMatrix(merged);
    }

	execution();

    MPI_Finalize();
    return 0;
}
