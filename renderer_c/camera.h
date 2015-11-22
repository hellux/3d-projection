#include <math.h>
#include <stdlib.h>

struct Camera {
    double pos[3];
    double rot[2];
    double array_size[2];
    double focal_length;
    double pixel_size;
    double rotation_matrix_x[3][3];
    double rotation_matrix_y[3][3];
};

struct Camera* camera_create(double pos[], double rot[], int res[], 
                             double fov, double focal_lengt);
static void calc_array_size(int res[], double fov, double array_size[]);
static void calc_rotation_matrices(double rot[], double rot_x[][3],
                                   double rot_y[][3]);
