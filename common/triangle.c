#include "triangle.h"

void triangle_create(double** vertices,
                     int indices[],
                     struct Object* O,
                     struct Triangle* T) {
    T->object = O;
    triangle_set_vertices(T, vertices, indices);
    triangle_calc_center(T);
    triangle_calc_vectors(T);
    triangle_calc_barycentric(T);
    triangle_calc_signed_distance(T);
}

void triangle_set_color(struct Triangle* T, double light[]) {
    double intensity = dot_product(T->normal, light) /
                (magnitude(T->normal)*magnitude(light));
    uint8_t hsl[3];
    rgb_to_hsl(T->object->color, hsl);
    hsl[2] = fabs(intensity) * 0xFF;
    hsl_to_rgb(hsl, T->color);
}

void triangle_set_vertices(struct Triangle* T,
                           double** vertices,
                           int indices[]) {
    for (int i = 0; i < 3; i++) {
        T->indices[i] = indices[i];
        for (int j = 0; j < 3; j++) {
            T->vertices[j][i] = vertices[indices[j]][i];
        }
    }
}

void triangle_calc_center(struct Triangle* T) {
    double sum[3];
    vector_add(T->vertices[0], T->vertices[1], sum);
    vector_add(sum, T->vertices[2], sum);
    vector_multiply(1.0/3.0, sum, T->center);
}

void triangle_calc_vectors(struct Triangle* T) {
    /* plane equation:
     * ax + by + cz + d = 0
     * plane normal:
     * n = (a, b, c) = (P3-P1) x (P2-P1) */
    vector_subtract(T->vertices[1], T->vertices[0], T->Q1);
    vector_subtract(T->vertices[2], T->vertices[0], T->Q2);
    cross_product(T->Q1, T->Q2, T->normal);
}

void triangle_calc_barycentric(struct Triangle* T) {
    double mat[][2] = {{ dot_product(T->Q1, T->Q1), dot_product(T->Q1, T->Q2) },
                       { dot_product(T->Q1, T->Q2), dot_product(T->Q2, T->Q2) }};
    matrix_inverse_2x2(mat, T->barycentric_matrix);
}

void triangle_calc_signed_distance(struct Triangle* T) {
    T->signed_distance = -dot_product(T->normal, T->vertices[0]);
}

void triangle_calc_camera_depth(struct Triangle* T, struct Camera* C) {
    T->depth = points_distance(C->pos, T->center);
}

int triangle_compare_depth(const void *t1, const void* t2) {
    /* return
     * z1 < z2 : 1
     * z1 > z2 : -1
     * z1 = z2 : 0 */
    const struct Triangle** dt1 = (const struct Triangle**) t1;
    const struct Triangle** dt2 = (const struct Triangle**) t2;
    return ((*dt1)->depth < (*dt2)->depth) - ((*dt1)->depth > (*dt2)->depth);
}

