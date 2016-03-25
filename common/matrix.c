#include "matrix.h"

void matrix_product(size_t rows_A, size_t cols_A, double A[][cols_A],
                    size_t rows_B, size_t cols_B, double B[][cols_B],
                                                  double C[][cols_B]) {

    /* Matrix product:
     * C = A*B */
    
    if (cols_A != rows_B) {
        fprintf(stderr, "Matrices can't multiply!\n");
        return;
    }
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

void matrix_inverse_2x2(double A[][2], double B[][2]) {
    /* B = A^-1, A must be a 2x2 matrix:
     * C is the cofactor adjugate of A
     *  A = | a b |     C = | d -b |
     *      | c d |         | -c a |
     *
     *  |A| = a*d - b*c
     *
     *  A^-1 = 1/|A| * C
     *
     * first, swap a with d then negate b and d to create matrix C.
     * next, multiply the matrix C with the inverse of the determinant of 
     * the original matrix A. */
    
    double determinant_A = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    
    if (determinant_A != 0) {
        double C[][2] = { {  A[1][1], -A[0][1] },
                          { -A[1][0],  A[0][0] } };

        /* multiply and store result in output matrix */
        matrix_scalar(1/determinant_A, 2, 2, C, B);
    } 
    else {
        fprintf(stderr, "renderer: inverse of 2x2 matrix does not exist\n");
    }
}

void matrix_inverse_3x3(double A[][3], double B[][3]) {
    /* B = A^-1, A must be a 3x3 matrix
     * B = 1/|A| * C^T
     *
     * C^T is the transpose of the A's cofactor matrix, can
     * also be expressed as the adjugate of A
     *
     *     | a b c |     | A B C |   | +(e*i-f*h) -(d*i-f*g) +(d*h-e*g) |
     * A = | d e f | C = | D E F | = | -(b*i-c*h) +(a*i-c*g) -(a*h-b*g) |
     *     | g h i |     | G H I |   | +(b*f-c*e) -(a*f-c*d) +(a*e-b*d) |
     *
     *       | A D G |
     * C^T = | B E H |
     *       | C F I |
     * 
     * |A|:
     *
     * a   b   c   a   b
     *   \   X   X   /
     * d   e   f   d   e     |A| = a*e*i + b*f*g + c*d*h
     *   /   X   X   \           - c*e*g - a*f*h - b*d*i
     * g   h   i   g   h 
     */
    
    /* calculate determinant |A| */
    double determinant_A = 0;
    for (int i = 0; i < 3; i++) {
        double product_pos = 1, product_neg = 1;
        for (int j = 0; j < 3; j++) {
            product_pos *= A[j][(j+i)%3];
            product_neg *= A[j][(2-j+i)%3];
        }
        determinant_A += product_pos - product_neg;
    }
    /* perform rest of calculations only if |A| is non-zero */
    if (determinant_A != 0) {
        /* adjugate of A */
        double CT[][3] = {{A[1][1]*A[2][2]-A[1][2]*A[2][1],     /*A*/
                           A[0][2]*A[2][1]-A[0][1]*A[2][2],     /*D*/
                           A[0][1]*A[1][2]-A[0][2]*A[1][1]},    /*G*/

                          {A[1][2]*A[2][0]-A[1][0]*A[2][2],     /*B*/
                           A[0][0]*A[2][2]-A[0][2]*A[2][0],     /*E*/
                           A[0][2]*A[1][0]-A[0][0]*A[1][2]},    /*H*/

                          {A[1][0]*A[2][1]-A[1][1]*A[2][0],     /*C*/
                           A[0][1]*A[2][0]-A[0][0]*A[2][1],     /*F*/
                           A[0][0]*A[1][1]-A[0][1]*A[1][0]}};   /*I*/
        /* multiply and store result in output matrix */
        matrix_scalar(1/determinant_A, 3, 3, CT, B);
    }
    else {
        fprintf(stderr, "renderer: inverse of 3x3 matrix does not exist\n");
        matrix_print(3, 3, A);
    }
}

void matrix_rotation_x(double cos_c, double sin_c, double R[][3]) {
    /* rotation matrix for x axis (yaw):
     *         | 1   0      0       |
     * Rx(c) = | 0   cos(c) -sin(c) |
     *         | 0   sin(c) cos(c)  | */
    R[0][0] = 1; R[0][1] = 0;     R[0][2] = 0;
    R[1][0] = 0; R[1][1] = cos_c; R[1][2] = -sin_c;
    R[2][0] = 0; R[2][1] = sin_c; R[2][2] = cos_c;
}

void matrix_rotation_y(double cos_b, double sin_b, double R[][3]) {
    /* rotation matrix for y axis (pitch)
     *         | cos(b)  0   sin(b) |
     * Ry(b) = | 0       1   0      |
     *         | -sin(b) 0   cos(b) | */
    R[0][0] = cos_b;  R[0][1] = 0; R[0][2] = sin_b;
    R[1][0] = 0;      R[1][1] = 1; R[1][2] = 0;
    R[2][0] = -sin_b; R[2][1] = 0; R[2][2] = cos_b;
}

void matrix_rotation_z(double cos_a, double sin_a, double R[][3]) {
    /* rotation matrix for z axis (roll)
     *         | cos(a) -sin(a) 0 |
     * Rz(a) = | sin(a) cos(a)  0 |
     *         | 0      0       1 | */
    R[0][0] = cos_a; R[0][1] = -sin_a; R[0][2] = 0;
    R[1][0] = sin_a; R[1][1] = cos_a;  R[1][2] = 0;
    R[2][0] = 0;     R[2][1] = 0;      R[2][2] = 1;
}

void matrix_print(size_t rows_A, size_t cols_A, double A[][cols_A]) {
    printf("\n");
    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_A; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
