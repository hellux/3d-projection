#include "matrix.h"

void matrix_product(size_t rows_A, size_t cols_A, double A[][cols_A],
                    size_t rows_B, size_t cols_B, double B[][cols_B],
                                                  double C[][cols_B]) {

    /* Matrix product:
     * C = A*B */
    
    if (cols_A != rows_B) fprintf(stderr, "Matrices can't multiply!\n");
    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_B; j++) {
            C[i][j] = 0;
            for (int k = 0; k < cols_A; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matrix_scalar(double m,
                   size_t rows_A, size_t cols_A, double A[][cols_A],
                                                 double B[][cols_A]) {
    /* Scalar multiplication:
     * B = m*A */
    for (int i = 0; i < rows_A; i++)
        for (int j = 0; j < cols_A; j++)
            B[i][j] = m * A[i][j];
}

void matrix_inverse(double A[][2], double B[][2]) {
    /* B = A^-1, A must be a 2x2 matrix:
     *  A = | a b |     C = | d -b |
     *      | c d |         | -c a |
     *
     *  detA = a*d - b*c
     *
     *  A^-1 = 1/detA * C
     *
     * first, swap a with d then negate b and d to create matrix C.
     * next, multiply the matrix C with the inverse of the determinant of 
     * the original matrix A. */

    double C[][2] = { {  A[1][1], -A[0][1] },
                      { -A[1][0],  A[0][0] } };
    double determinant_A = A[0][0]*A[1][1] - A[0][1]*A[1][0];

    matrix_scalar(1/determinant_A, 2, 2, C, B);
}

void matrix_print(size_t rows_A, size_t cols_A, double A[][cols_A]) {
    printf("\n");
    for (int i = 0; i < cols_A; i++) {
        for (int j = 0; j < rows_A; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
