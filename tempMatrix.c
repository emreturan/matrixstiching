#include "tempMatrix.h"

struct tempMatrix *createTempMatrix(struct matrix *srcMatrix, int width, int height, int posx, int posy){
    struct tempMatrix *tempMatrix = (struct tempMatrix *) malloc(sizeof(struct tempMatrix));

    tempMatrix->matrix = createMatrix(width, height);

    tempMatrix->realWidth = srcMatrix->width;
    tempMatrix->realHeight = srcMatrix->height;

    tempMatrix->posx = posx;
    tempMatrix->posy = posy;

    int i, j;
    for (i = 0; i < srcMatrix->height; i++){
        for (j = 0; j < srcMatrix->width; j++){
            if (bitset_get(srcMatrix->matrix[i], j) == true) bitset_set(tempMatrix->matrix->matrix[posy + i], posx + j);
        }
    }

    return tempMatrix;
}

void freeTempMatrix(struct tempMatrix **tempMatrix){
    freeMatrix(&((*tempMatrix)->matrix));
    free(*tempMatrix);
    *tempMatrix = NULL;
}
