#ifndef triangle_h
#define triangle_h

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "matrix.h"

struct Triangle {
    /* vertices / points */
    double P1[3];
    double P2[3];
    double P3[3];

    /* vectors & normal */
    double Q1[3];
    double Q2[3];
    double N[3];

    /* matrix */
    double M[2][2];

    double D;
};

void triangle_create(double vertices[][3],
                     int indices[],
                     struct Triangle* T);

#endif
