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

double points_distance(double A[], double B[]) {
    /* distance between two points A, B */
    double AB[3];
    vector_subtract(B, A, AB);
    return magnitude(AB);
}

void vector_copy(double u[], double v[]) {
    for (int i = 0; i < 3; i++) v[i] = u[i];
}


bool normal_faces_point(double N[], double P[], double S[]) {
    /* determine if the normal N is pointing towards the side where the point S lies
     *
     *       A  .____  D
     *         /|\   ----___. S
     *          |         _/
     *        N |       _/
     * ---------P-----_/--------
     *      N_n |   _/ D_n
     *          | _/
     *         \!/
     *         A_n
     *
     * return |D| < |D_inv| */

    double A[3], A_n[3], magD, magD_n;
    vector_add(P, N, A);                /* A = P + N */
    vector_subtract(P, N, A_n);         /* A_n = P - N */
    magD = points_distance(A, S);       /* |D|: distance between A, S */
    magD_n = points_distance(A_n, S);   /* |D_n|: distance between A_n, S */
    return magD < magD_n;               /* |D| < |D_n| */
}

void vector_print(double u[]) {
    printf("\n( ");
    for (int i = 0; i < 3; i++) {
        printf("%f ", u[i]);
    }
    printf(")\n");
}
