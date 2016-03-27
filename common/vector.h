#ifndef vector_h
#define vector_h

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "matrix.h"

void vector_add(double u[], double v[], double w[]);
void vector_subtract(double u[], double v[], double w[]);
void vector_multiply(double m, double u[], double v[]);
void cross_product(double u[], double v[], double w[]);
double dot_product(double u[], double v[]);
double magnitude(double u[]);
double points_distance(double A[], double B[]);
void vector_copy(double u[], double v[]);
void vector_print(double u[]);
void vector_transform(double matrix[][3], double u[]);
bool normal_faces_point(double N[], double P[], double S[]);

#endif
