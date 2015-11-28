#include "vector.h"

/* all functions only work for vectors in R^3 */

void vector_add(double u[], double v[], double w[]) {
    /* w = u + v */
    for (int i = 0; i < 3; i++) w[i] = u[i] + v[i];
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

void vector_print(double u[]) {
    printf("\n( ");
    for (int i = 0; i < 3; i++) {
        printf("%f ", u[i]);
    }
    printf(")\n");
}
