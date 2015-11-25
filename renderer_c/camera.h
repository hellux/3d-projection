#ifndef camera_h
#define camera_h

#include <math.h>
#include <stdlib.h>

#include "matrix.h"

struct Camera {
    double pos[3];
    double rot[3];
    int res[2];
    double fov;
    double array_size[2];
    double focal_length;
    double pixel_size;
    double rotation_matrix[3][3];
};

struct Camera* camera_create(double pos[], double rot[], int res[], 
                             double fov, double focal_length);
static void calc_array_size(struct Camera* C);
static void calc_rotation_matrix(struct Camera* C);
void camera_calc_direction(struct Camera C, int row, int col, double V[]);

#endif
