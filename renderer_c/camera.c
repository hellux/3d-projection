#include "camera.h"

struct Camera* camera_create(double pos[], double rot[], int res[],
                             double fov, double focal_length) {
    struct Camera* C = malloc(sizeof(struct Camera));
    
    for (int i = 0; i < 3; i++) C->pos[i] = pos[i];
    for (int i = 0; i < 2; i++) C->rot[i] = rot[i];

    C->focal_length = focal_length;
    calc_array_size(res, fov, C->array_size);
    C->pixel_size = C->array_size[0] / res[0];
    calc_rotation_matrices(rot, C->rotation_matrix_x, C->rotation_matrix_y);

    return C;
}

static void calc_array_size(int res[], double fov, double array_size[]) {
    array_size[0] = fov * sin(fov/2);
    array_size[1] = array_size[0] * (res[1]/res[0]);
}

static void calc_rotation_matrices(double rot[], double rot_x[][3],
                                   double rot_y[][3]) {
   /* | 1   0       0       |
    * | 0   cos(a)  sin(a)  |
    * | 0   -sin(a) cos(a)  | */
   rot_x[0][0] = 1; 
   rot_x[0][1] = 0;
   rot_x[0][2] = 0;
   rot_x[1][0] = 0;
   rot_x[1][1] = cos(rot[0]);
   rot_x[1][2] = sin(rot[0]);
   rot_x[2][0] = 0;
   rot_x[2][1] = -sin(rot[0]);
   rot_x[2][2] = cos(rot[0]);
   
   /* | cos(b)  0   -sin(b) |
    * | o       1   0       |
    * | sin(b)  0   cos(b)  | */
   rot_y[0][0] = cos(rot[1]);
   rot_y[0][1] = 0;
   rot_y[0][2] = -sin(rot[1]);
   rot_y[1][0] = 0;
   rot_y[1][1] = 1;
   rot_y[1][2] = 0;
   rot_y[2][0] = sin(rot[1]);
   rot_y[2][1] = 0;
   rot_y[2][2] = cos(rot[1]);
}
