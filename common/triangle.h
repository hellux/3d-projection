#ifndef triangle_h
#define triangle_h

#include <stdio.h>
#include <stdlib.h>

#include "../common/vector.h"
#include "../common/matrix.h"
#include "../common/camera.h"

struct Triangle {
    double vertices[3][3];
    int indices[3];

    double center[3];
    double depth;

    double normal[3];
    double signed_distance;
    double Q1[3], Q2[3];

    double barycentric_matrix[2][2];

    struct Object* object;
};

void triangle_create(double vertices[][3],
                     int indices[],
                     struct Object* O,
                     struct Triangle* T);
void triangle_set_vertices(struct Triangle* T, double vertices[][3], int indices[]);
void triangle_calc_center(struct Triangle* T);
void triangle_calc_vectors(struct Triangle* T);
void triangle_calc_barycentric(struct Triangle* T);
void triangle_calc_signed_distance(struct Triangle* T);
void triangle_calc_camera_depth(struct Triangle* T, struct Camera* C);

#endif
