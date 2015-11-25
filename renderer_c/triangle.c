#include "triangle.h"

void triangle_create(double vertices[][3],
                     int indices[],
                     struct Triangle* T) {
    for (int i = 0; i < 3; i++) {
        T->P1[i] = vertices[indices[0]][i];
        T->P2[i] = vertices[indices[1]][i];
        T->P3[i] = vertices[indices[2]][i];
    }
    vector_subtract(T->P2, T->P1, T->Q1);
    vector_subtract(T->P3, T->P1, T->Q2);

    cross_product(T->Q1, T->Q2, T->N);

    double mat[][2] = {{ dot_product(T->Q1, T->Q1), dot_product(T->Q1, T->Q2) },
                       { dot_product(T->Q1, T->Q2), dot_product(T->Q2, T->Q2) }};
    matrix_inverse(mat, T->M);

    T->D = -dot_product(T->N, T->P1);
}


