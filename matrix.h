#include <stdlib.h>
#include <stdbool.h>
#include "bitset.h"

struct matrix{
    bitset_t **matrix;
    int width;
    int height;
};

struct matrix *createMatrix(int, int);
void printMatrix(struct matrix *);
void fillMatrix(struct matrix *);
void freeMatrix(struct matrix **);
int **findOnes(struct matrix *);
