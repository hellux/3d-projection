#include "camera.h"

struct Camera* camera_create(double pos[], double rot[], int res[],
                             double fov, double focal_length) {
    struct Camera* C = malloc(sizeof(struct Camera));
   
    /* copy arrays to struct*/ 
    for (int i = 0; i < 3; i++) {
        C->pos[i] = pos[i];
        C->rot[i] = fmod(rot[i], 2*M_PI);
    }
    for (int i = 0; i < 2; i++) {
        if (res[i] > 0) C->res[i] = res[i];
        else {
            printf("Invalid resolution.\n");
            return NULL;
        }
    }

    /* copy variables to struct */
    if (fov > 0 && fov < M_PI) C->fov = fov;
    else {
        printf("Invalid field of view.\n");
        return NULL;
    }
    if (focal_length > 0) C->focal_length = focal_length;
    else {
        printf("Invalid focal length.\n");
        return NULL;
    }

    /* perform one-time calculations */ 
    calc_array_size(C);
    C->pixel_size = C->array_size[0] / res[0];
    calc_rotation_matrix(C);

    return C;
}

void calc_array_size(struct Camera* C) {
    /*     width
     * |-----------|   [m]
     *  ___________ _
     *  \  |_|    / |
     *   \   |   /  |
     *    \  |  /   | focal length
     *     \v| /    |
     *      \|/     |
     *       C      -
     *
     * tan(v) = opposite / adjacent
     * tan(v) = (width/2) / focal length = width/(2*focal length)
     * width = tan(v) * 2 * focal length
     * v = fov/2
     * width = tan(fov/2) * 2 * focal_length
     *
     *                      res_x
     *   width   [m]      __________  [px]
     *  _______          |          |
     * |       | height  |          | res_y
     * |_______|         |__________| 
     *
     * height / width = res_y / res_x
     * height = width * res_y / res_x */ 

    C->array_size[0] = tan(C->fov/2) * 2 * C->focal_length;
    C->array_size[1] = C->array_size[0] * (C->res[1]/C->res[0]);
}

void calc_rotation_matrix(struct Camera* C) {
    double rot_x[3][3], rot_y[3][3], rot_z[3][3]; 

    /* rotation matrix for x axis (yaw):
     *         | 1   0       0       |
     * Rx(a) = | 0   cos(a)  sin(a)  |
     *         | 0   -sin(a)  cos(a) | */
    rot_x[0][0] = 1; rot_x[0][1] = 0;               rot_x[0][2] = 0;
    rot_x[1][0] = 0; rot_x[1][1] = cos(C->rot[0]);  rot_x[1][2] = sin(C->rot[0]);
    rot_x[2][0] = 0; rot_x[2][1] = -sin(C->rot[0]); rot_x[2][2] = cos(C->rot[0]);
    
    /* rotation matrix for y axis (pitch)
     *         | cos(b)  0   -sin(b) |
     * Ry(b) = | 0       1   0       |
     *         | sin(b)  0   cos(b)  | */
    rot_y[0][0] = cos(C->rot[1]);  rot_y[0][1] = 0; rot_y[0][2] = -sin(C->rot[1]);
    rot_y[1][0] = 0;               rot_y[1][1] = 1; rot_y[1][2] = 0;
    rot_y[2][0] = sin(C->rot[1]);  rot_y[2][1] = 0; rot_y[2][2] = cos(C->rot[1]);

    /* rotation matrix for z axis (roll)
     *         | cos(c) sin(c)   0 |
     * Rz(c) = | -sin(c) cos(c)  0 |
     *         | 0      0        1 | */
    rot_z[0][0] = cos(C->rot[2]);  rot_z[0][1] = sin(C->rot[2]);  rot_z[0][2] = 0;
    rot_z[1][0] = -sin(C->rot[2]); rot_z[1][1] = cos(C->rot[2]);  rot_z[1][2] = 0;
    rot_z[2][0] = 0;               rot_z[2][1] = 0;               rot_z[2][2] = 1;
    
    /* combined rotation matrix
     * R = Rx(a) * Ry(b) * Rz(c) */
    double rot_z_rot_y[3][3];
    matrix_product(3, 3, rot_z,
                   3, 3, rot_y,
                   rot_z_rot_y);
    matrix_product(3, 3, rot_z_rot_y,
                   3, 3, rot_x,
                   C->rotation_matrix);
}

void camera_calc_direction(struct Camera C, int row, int col, double V[]) {
    /*
     *    focal length         |---- width ----|
     *  |---------------|  c ^         ^ y        [m]  
     *                       |  _______|_______         _
     *  ^ y             |    H |_|_|_|_|_|_|_|_|        |
     *  |   z           |    : |_|_|_|_|_|_|_|_|   x    |
     *  C -->  -  -  -  |    2 |_|_|_|_C_|_|_|_|--->  height
     *                  |    1 |_|_|_|_|_|_|_|_|        |
     *                  |    0 |_|_|_|_|_|_|_|_|        |
     *                          0 1 2 3 4 ... W -->     -
     *                                            r
     * C: camera
     * W, H: resolution width, height
     * r, c: row, column
     *
     * First, create a vertex for a point in an unrotated array
     * with origo located inside the camera. The vertex will be
     * equal to the vector representing the direction of the ray
     * once rotated with camera rotation.
     * V0 = (x, y, z)
     * x = -width  / 2 + (col+0.5) * pixel_size
     * y = -height / 2 + (row+0.5) * pixel_size
     * z = focal_length
     *
     * then add the camera's rotation to the vector by multiplying the vector
     * with the camera's rotation matrix:
     *
     * |x'|     |x|
     * |y'| = R |y| = R * V^T
     * |z'|     |z|
     *
     * V = (x', y', z') */

    double unrotated_V[3][1], rotated_V[3][1];

    unrotated_V[0][0] = -C.array_size[0]/2 + (col+0.5)*C.pixel_size;
    unrotated_V[1][0] = -C.array_size[1]/2 + (row+0.5)*C.pixel_size;
    unrotated_V[2][0] = C.focal_length;

    matrix_product(3, 3, C.rotation_matrix,
                   3, 1, unrotated_V,
                         rotated_V);

    /* copy coordinates from column matrix to row vector V */
    for (int i = 0; i < 3; i++) V[i] = rotated_V[i][0];
}
