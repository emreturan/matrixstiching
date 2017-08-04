#include <stdbool.h>
#include "matrix.h"

struct tempMatrix{
    struct matrix *matrix;
    int realWidth;
    int realHeight;
    int posx;
    int posy;
};

struct tempMatrix *createTempMatrix(struct matrix *, int, int, int, int);
void freeTempMatrix(struct tempMatrix **);
