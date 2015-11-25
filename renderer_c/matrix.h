#ifndef matrix_h
#define matrix_h

#include <stdio.h>
#include <math.h>

void matrix_product(size_t rows_A, size_t cols_A, double A[][cols_A],
                    size_t rows_B, size_t cols_B, double B[][cols_B],
                                                  double C[][cols_B]);
void matrix_scalar(double m,
                   size_t rows_A, size_t cols_A, double A[][cols_A],
                                                 double B[][cols_A]);
void matrix_inverse(double A[][2], double B[][2]);
void matrix_print(size_t rows_A, size_t cols_A, double A[][cols_A]);

#endif
