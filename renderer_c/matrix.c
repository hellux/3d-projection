#include "matrix.h"

void matrix_product(size_t rows_A, size_t cols_A, double A[][cols_A],
                    size_t rows_B, size_t cols_B, double B[][cols_B],
                                                  double C[][cols_B]) {

    /* Matrix product:
     * C = A*B */
    
    if (cols_A != rows_B) printf("Matrices can't multiply!");
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

bool matrix_equal(size_t rows, size_t cols, double A[][cols],
											double B[][cols]) {
	/* A == B */
	bool equal = true;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			equal = (A[i][j] == B[i][j] ? equal : false);	
		}
	}	
}

void matrix_test(void) {
	double A[][3] = {{1, 2, 3},
				     {4, 5, 6},
				     {7, 8, 9}};
	double B[][3] = {{10, 11, 12},
					 {13, 14, 15},
					 {16, 17, 18}};
	double C[3][3];
	double D[][2] = {{1, 2},
					 {3, 4}};
	double E[2][2];
	double mul[][3] = {{84,  90,  96 },
					   {201, 216, 231},
					   {318, 342, 366}};
	double scl[][3] = {{2,  4,  6 },
					   {8,  10, 12},
					   {14, 16, 18}};
	double inv[][2] = {{-2,    1  },
					   { 3/2, -1/2}};

	matrix_product(3, 3, A, 3, 3, B, C);
	assert(matrix_equal(3, 3, C, mul));
	matrix_scalar(2, 3, 3, A, D);
	assert(matrix_equal(3, 3, C, scl));
	matrix_inverse(D, E);
	assert(matrix_equal(2, 2, E, inv));
}

