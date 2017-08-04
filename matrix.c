#include "matrix.h"


struct matrix *createMatrix(int width, int height){
    struct matrix *matrix = (struct matrix *) malloc(sizeof(struct matrix));

    matrix->matrix = (bitset_t **) malloc(sizeof(bitset_t *) * height);

    int i;
    for (i = 0; i < height; i++){
        matrix->matrix[i] = bitset_create_with_capacity(width);
    }

    matrix->width = width;
    matrix->height = height;
    return matrix;
}

void printMatrix(struct matrix *matrix){
    int i, j;
    for (i = 0; i < matrix->height; i++){
        for (j = 0; j < matrix->width; j++){
            printf("%d ", bitset_get(matrix->matrix[i], j));
        }
        printf("\n");
    }
    printf("\n\n");
}

void fillMatrix(struct matrix *matrix){


    int i, j;
    for (i = 0; i < matrix->height; i++){
        for (j = 0; j < matrix->width; j++){
            int randd = rand()%2;
            if (randd == 0){
                bitset_set(matrix->matrix[i], j);
            }
        }
    }
}

void freeMatrix(struct matrix **matrix){
    int i;
    for (i = 0; i < (*matrix)->height; i++)
        bitset_free((*matrix)->matrix[i]);
    free((*matrix)->matrix);
    free(*matrix);
    *matrix = NULL;
}

int **findOnes(struct matrix *matrix){
    int **ones = (int **) malloc (sizeof(int *) * matrix->height);
    int i, trueCounter;
    for (i = 0; i < matrix->height; i++){
        ones[i] = (int*) malloc(sizeof(int) * bitset_count(matrix->matrix[i]) + 1);

        ones[i][0] = bitset_count(matrix->matrix[i]) + 1;

        trueCounter = 1;

        size_t j;
        for (j = 0; nextSetBit(matrix->matrix[i], &j); j++){
            ones[i][trueCounter] = j;
            trueCounter++;
        }
    }
    return ones;
}
