#include <mpi.h>
#include <stdio.h>
#include "tempMatrix.h"

#define MASTER 0

struct mergeSpace{
    struct tempMatrix *dynamicPos;
    struct tempMatrix *staticPos;
};

struct mergeSpace *createMergeSpace(struct tempMatrix *, struct tempMatrix *);
struct mergeSpace *getBestMergeSolution(struct matrix *, struct matrix *);
int *getMergePoint(struct mergeSpace *);
struct matrix *merge(struct mergeSpace *);
int mergeFillAssistant(struct mergeSpace *, int, int);
int getOptimalWidth(struct matrix *, struct matrix *, int);
int getOptimalHeight(struct matrix *, struct matrix *, int);
int getOptimalDynx(struct matrix *, int);
int getOptimalDyny(struct matrix *, int);
int getOptimalStax(struct matrix *, int);
int getOptimalStay(struct matrix *, int);
void freeMergeSpace(struct mergeSpace **);
