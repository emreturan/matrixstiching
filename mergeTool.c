#include "mergeTool.h"

struct mergeSpace *createMergeSpace(struct tempMatrix *dynamicPos, struct tempMatrix *staticPos){
    struct mergeSpace *mergeSpace = (struct mergeSpace *) malloc(sizeof (struct mergeSpace));
    mergeSpace->dynamicPos = dynamicPos;
    mergeSpace->staticPos = staticPos;
    return mergeSpace;
}

struct mergeSpace *getBestMergeSolution(struct matrix *dynamicPos, struct matrix *staticPos){
	int rank, width, height;
	struct mergeSpace *mergeSpace = NULL;
	struct tempMatrix *_dynamicPos, *_staticPos;
	_dynamicPos = NULL;
	_staticPos = NULL;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == MASTER){
		width = getOptimalWidth(dynamicPos, staticPos, 0);
		height = getOptimalHeight(dynamicPos, staticPos, 0);

		_dynamicPos = createTempMatrix(dynamicPos, width, height, getOptimalDynx(dynamicPos, 0), getOptimalDyny(dynamicPos, 0));
		_staticPos = createTempMatrix(staticPos, width, height, getOptimalStax(dynamicPos, 0), getOptimalStay(dynamicPos, 0));

		mergeSpace = createMergeSpace(_dynamicPos, _staticPos);
	}

    int *coord = getMergePoint(mergeSpace);

    if (rank == MASTER){
        printf("x:%d - y:%d \n\n", coord[0], coord[1]);

        freeMergeSpace(&mergeSpace);

        width = getOptimalWidth(dynamicPos, staticPos, coord[0]);
        height = getOptimalHeight(dynamicPos, staticPos, coord[1]);
        _dynamicPos = createTempMatrix(dynamicPos, width, height, getOptimalDynx(dynamicPos, coord[0]), getOptimalDyny(dynamicPos, coord[1]));
        _staticPos = createTempMatrix(staticPos, width, height, getOptimalStax(dynamicPos, coord[0]), getOptimalStay(dynamicPos, coord[1]));

        mergeSpace = createMergeSpace(_dynamicPos, _staticPos);
    }

    return mergeSpace;
}

int *getMergePoint(struct mergeSpace *mergeSpace){
	int rank, worldRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldRank);
	int **dynamicOnes = NULL;
	int **staticOnes = NULL;

	int height, width;
	if (rank == MASTER){
		height = mergeSpace->dynamicPos->matrix->height;
		width = mergeSpace->dynamicPos->matrix->width;
	}
	MPI_Bcast(&height, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&width, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER){
		dynamicOnes = findOnes(mergeSpace->dynamicPos->matrix);
		staticOnes = findOnes(mergeSpace->staticPos->matrix);
	} else {
		dynamicOnes = (int **) malloc (sizeof(int *) * height);
		staticOnes = (int **) malloc (sizeof(int *) * height);
	}

	register int i, j, k, l;
	int dynSize, staSize;
	for (i = 0; i < height; i++){
		if (rank == MASTER){
			dynSize = dynamicOnes[i][0] + 1;
			staSize = staticOnes[i][0] + 1;
		}
		MPI_Bcast(&dynSize, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&staSize, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
		if (rank != MASTER){
			dynamicOnes[i] = (int *) malloc (sizeof(int) * dynSize);
			staticOnes[i] = (int *) malloc (sizeof(int) * staSize);
		}
		MPI_Bcast(dynamicOnes[i], dynSize, MPI_INT, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(staticOnes[i], staSize, MPI_INT, MASTER, MPI_COMM_WORLD);
	}

	int **mergePoints = (int **) malloc(sizeof(int *) * height);

	for (i = 0; i < height; i++){
		mergePoints[i] = (int *) malloc(sizeof(int) * width);
		memset(mergePoints[i], 0, sizeof(int) * width);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for (i = 0; i < height; i++){
		if (i % worldRank == rank){
			for (j = 1; j < dynamicOnes[i][0]; j++){
				for (k = 0; k < height; k++){
					for (l = 1; l < staticOnes[k][0]; l++){
						mergePoints[k - i][staticOnes[k][l] - dynamicOnes[i][j]]++;
					}
				}
			}
			//printf("%d\n", i);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int **mergePointsSum = (int **) malloc (sizeof(int *) * height);
	for (i = 0; i < height; i++){
		mergePointsSum[i] = (int *) malloc (sizeof(int) * width);
		memset(mergePointsSum[i], 0, sizeof(int) * width);
		MPI_Reduce(mergePoints[i], mergePointsSum[i], width, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
		free(mergePoints[i]);
	}

	int *maxCoord = NULL;

	if (rank == MASTER){
		int max = -1;
		maxCoord = (int *) malloc(sizeof(int) * 2);
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				if (mergePointsSum[i][j] > max){
					max = mergePointsSum[i][j];
					maxCoord[0] = j;
					maxCoord[1] = i;
				}
			}
		}
	}

	free(mergePoints);

	return maxCoord;
}

struct matrix *merge(struct mergeSpace *mergeSpace){
    int lowerx = mergeSpace->dynamicPos->posx < mergeSpace->staticPos->posx ? \
                    mergeSpace->dynamicPos->posx : mergeSpace->staticPos->posx;
    int higherx = mergeSpace->dynamicPos->posx + mergeSpace->dynamicPos->realWidth > mergeSpace->staticPos->posx + mergeSpace->staticPos->realWidth? \
                    mergeSpace->dynamicPos->posx + mergeSpace->dynamicPos->realWidth: \
                    mergeSpace->staticPos->posx + mergeSpace->staticPos->realWidth;
    int width = higherx - lowerx;

    int lowery = mergeSpace->dynamicPos->posy < mergeSpace->staticPos->posy ? \
                    mergeSpace->dynamicPos->posy : mergeSpace->staticPos->posy;
    int highery = mergeSpace->dynamicPos->posy + mergeSpace->dynamicPos->realHeight > mergeSpace->staticPos->posy + mergeSpace->staticPos->realHeight? \
                    mergeSpace->dynamicPos->posy + mergeSpace->dynamicPos->realHeight : \
                    mergeSpace->staticPos->posy + mergeSpace->staticPos->realHeight;
    int height = highery - lowery;
    struct matrix *matrix = createMatrix(width, height);

    int i, j, x, y;
    for (i = lowery, y = 0; i < highery; i++, y++){
        for (j = lowerx, x = 0; j < higherx; j++, x++){
            if (mergeFillAssistant(mergeSpace, j, i)){
                bitset_set(matrix->matrix[y], x);
            }
        }
    }

    return matrix;
}

int mergeFillAssistant(struct mergeSpace *mergeSpace, int x, int y){
    if (bitset_get(mergeSpace->dynamicPos->matrix->matrix[y], x) == true || bitset_get(mergeSpace->staticPos->matrix->matrix[y], x) == true) return 1;
    return 0;
}

int getOptimalWidth(struct matrix *dynamicPos, struct matrix *staticPos, int virDynPosx){
    int startx, endx;
    startx = virDynPosx < dynamicPos->width - 1 ? virDynPosx : dynamicPos->width - 1;
    endx = (virDynPosx + dynamicPos->width > dynamicPos->width + staticPos->width - 2) ? (virDynPosx + dynamicPos->width) : (dynamicPos->width + staticPos->width - 2);
    return endx - startx + 1;
}

int getOptimalHeight(struct matrix *dynamicPos, struct matrix *staticPos, int virDynPosy){
    int starty, endy;
    starty = virDynPosy < dynamicPos->height - 1 ? virDynPosy : dynamicPos->height - 1;
    endy = virDynPosy + dynamicPos->height > dynamicPos->height + staticPos->height - 2 ? virDynPosy + dynamicPos->height : dynamicPos->height + staticPos->height - 2;
    return endy - starty + 1;
}

int getOptimalDynx(struct matrix *dynamicPos, int virDynPosx){
    if (virDynPosx < dynamicPos->width - 1) return 0;
    return virDynPosx - (dynamicPos->width - 1);
}

int getOptimalDyny(struct matrix *dynamicPos, int virDynPosy){
    if (virDynPosy < dynamicPos->height - 1) return 0;
    return virDynPosy - (dynamicPos->height - 1);
}

int getOptimalStax(struct matrix *dynamicPos, int virDynPosx){
    if (virDynPosx > dynamicPos->width - 1) return 0;
    return (dynamicPos->width - 1) - virDynPosx;
}

int getOptimalStay(struct matrix *dynamicPos, int virDynPosy){
    if (virDynPosy > dynamicPos->height - 1) return 0;
    return (dynamicPos->height - 1) - virDynPosy;
}

void freeMergeSpace(struct mergeSpace **mergeSpace){
    freeTempMatrix(&((*mergeSpace)->dynamicPos));
    freeTempMatrix(&((*mergeSpace)->staticPos));
    free(*mergeSpace);
    *mergeSpace = NULL;
}
