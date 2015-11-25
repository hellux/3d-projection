#ifndef vector_h
#define vector_h

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

void vector_add(double u[], double v[], double w[]);
void vector_subtract(double u[], double v[], double w[]);
void vector_multiply(double m, double u[], double v[]);
void cross_product(double u[], double v[], double w[]);
double dot_product(double u[], double v[]);
double magnitude(double u[]);
void vector_print(double u[]);
bool vector_equal(double u[], double v[]);
void vector_test(void);

#endif
