#include "vector.h"

/* all functions only work for vectors in R^3 */

void vector_add(double u[], double v[], double w[]) {
    /* w = u + v */
    for (int i = 0; i < 3; i++)
        w[i] = u[i] + v[i];
}

void vector_subtract(double u[], double v[], double w[]) {
    /* w = u - v */
    for (int i = 0; i < 3; i++)
        w[i] = u[i] - v[i];
}

void vector_multiply(double m, double u[], double w[]) {
    /* v = m * u */
    for (int i = 0; i < 3; i++) w[i] = m * u[i];
}

void cross_product(double u[], double v[], double w[]) {
    /* w = u x v */
    w[0] = u[1]*v[2] - u[2]*v[1];
    w[1] = u[2]*v[0] - u[0]*v[2];
    w[2] = u[0]*v[1] - u[1]*v[0];
}

double dot_product(double u[], double v[]) {
    /* dot product / scalar product:
     * sum = u * v */
    double sum = 0;
    for (int i = 0; i < 3; i++) sum += u[i] * v[i];
    return sum;
}

double magnitude(double u[]) {
    /* magnitude of u: 
     * |u| = sqrt( x^2 + y^2 + z^2 ) */
    return sqrt(pow(u[0], 2) +
                pow(u[1], 2) +
                pow(u[2], 2));
}

double points_distance(double A[], double B[]) {
    /* distance between two points A, B */
    double AB[3];
    vector_subtract(B, A, AB);
    return magnitude(AB);
}

void vector_copy(double u[], double v[]) {
    for (int i = 0; i < 3; i++) v[i] = u[i];
}

void vector_transform(double matrix[][3], double u[]) {
    double mat_u[][1] = {{u[0]},
                         {u[1]},
                         {u[2]}};
    double mat_u_res[3][1];

    matrix_product(3, 3, matrix,
                   3, 1, mat_u,
                   mat_u_res);

    u[0] = mat_u_res[0][0];
    u[1] = mat_u_res[1][0];
    u[2] = mat_u_res[2][0];
}

void vector_print(double u[]) {
    printf("\n( ");
    for (int i = 0; i < 3; i++) {
        printf("%f ", u[i]);
    }
    printf(")\n");
}
