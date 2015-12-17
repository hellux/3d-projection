#include "camera.h"

struct Camera* camera_create(double pos[], double rot[], int res[],
                             double fov, double focal_length) {
    struct Camera* C = malloc(sizeof(struct Camera));
    for (int i = 0; i < 3; i++) {
        C->pos[i] = pos[i];
        C->rot[i] = fmod(rot[i], M_PI*2);
        C->acc[i] = 0;
        C->vel[i] = 0;
    }
    for (int i = 0; i < 2; i++) {
        if (res[i] > 0) C->res[i] = res[i];
        else {
            fprintf(stderr, "previewer: invalid resolution -- %dx%dpx\n",
                    res[0], res[1]);
            return NULL;
        }
    }

    if (focal_length > 0) C->focal_length = focal_length;
    else {
        fprintf(stderr, "previewer: invalid focal length -- %f m\n", focal_length);
        return NULL;
    }
    if (fov > 0 && fov < M_PI) C->fov = fov;
    else {
        fprintf(stderr, "previewer: invalid field of view");
        return NULL;
    }

    camera_calc_array_size(C);
    C->pixel_size = C->array_width / res[0];
    camera_update_rotation(C);

    return C;
}

void camera_calc_array_size(struct Camera* C) {
    /*     width
     * |-----------|   [m]
     *  ___________ _
     *  \  |_.    / |
     *       .      |
     *    \  .  /   | focal length
     *      v.v     |
     *      \./     |
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

    C->array_width = tan(C->fov/2) * 2 * C->focal_length;
    C->array_height = C->array_width * C->res[1] / C->res[0];
}

void camera_calc_rotation_matrix(struct Camera* C) {
    double rot_x[3][3], rot_y[3][3], rot_z[3][3];

    /* rotation matrix for x axis (yaw):
     *         | 1   0      0       |
     * Rx(a) = | 0   cos(a) -sin(a) |
     *         | 0   sin(a) cos(a)  | */
    rot_x[0][0] = 1; rot_x[0][1] = 0;               rot_x[0][2] = 0;
    rot_x[1][0] = 0; rot_x[1][1] = C->frame_cos[0]; rot_x[1][2] = -C->frame_sin[0];
    rot_x[2][0] = 0; rot_x[2][1] = C->frame_sin[0]; rot_x[2][2] = C->frame_cos[0];

    /* rotation matrix for y axis (pitch)
     *         | cos(b)  0   sin(b) |
     * Ry(b) = | 0       1   0      |
     *         | -sin(b) 0   cos(b) | */
    rot_y[0][0] = C->frame_cos[1];  rot_y[0][1] = 0; rot_y[0][2] = C->frame_sin[1];
    rot_y[1][0] = 0;                rot_y[1][1] = 1; rot_y[1][2] = 0;
    rot_y[2][0] = -C->frame_sin[1]; rot_y[2][1] = 0; rot_y[2][2] = C->frame_cos[1];

    /* rotation matrix for z axis (roll)
     *         | cos(c) -sin(c) 0 |
     * Rz(c) = | sin(c) cos(c)  0 |
     *         | 0      0       1 | */
    rot_z[0][0] = C->frame_cos[2]; rot_z[0][1] = -C->frame_sin[2]; rot_z[0][2] = 0;
    rot_z[1][0] = sin(C->rot[2]);  rot_z[1][1] = C->frame_cos[2];  rot_z[1][2] = 0;
    rot_z[2][0] = 0;               rot_z[2][1] = 0;                rot_z[2][2] = 1;

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

void camera_calc_pixel_direction(struct Camera C, int row, int col, double V[]) {
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
     * V = (x', y', z')
     *
     *       |x'|              |x|
     * V^T = |y'| = R*V0^T = R*|y|
     *       |z'|              |z| */

    double unrotated_V[3][1], rotated_V[3][1];

    unrotated_V[0][0] = -C.array_width/2 + (col+0.5)*C.pixel_size;
    unrotated_V[1][0] = -C.array_height/2 + (row+0.5)*C.pixel_size;
    unrotated_V[2][0] = C.focal_length;

    matrix_product(3, 3, C.rotation_matrix,
                   3, 1, unrotated_V,
                         rotated_V);

    /* copy coordinates from column matrix to row vector V */
    for (int i = 0; i < 3; i++) V[i] = rotated_V[i][0];
}

void camera_set_slow(struct Camera* C, bool s) { C->slow = s; }
void camera_increase_fov(struct Camera* C) { camera_change_fov(C, true); }
void camera_decrease_fov(struct Camera* C) { camera_change_fov(C, false); }

void camera_change_fov(struct Camera* C, bool increase) {
    if (increase) {
        C->fov *= CAMERA_FOV_STEP_FACTOR;
    }
    else {
        C->fov /= CAMERA_FOV_STEP_FACTOR;
    }
    camera_calc_array_size(C);
}

void camera_move_forward(struct Camera* C) { 
    camera_move_horizontal(C, -CAMERA_SPEED, C->rot[1]);
}
void camera_move_back(struct Camera* C) {
    camera_move_horizontal(C, CAMERA_SPEED, C->rot[1]);
}
void camera_move_right(struct Camera* C) { 
    camera_move_horizontal(C, -CAMERA_SPEED, C->rot[1] + M_PI / 2);
}
void camera_move_left(struct Camera* C) {
    camera_move_horizontal(C, CAMERA_SPEED, C->rot[1] + M_PI / 2);
}
void camera_move_up(struct Camera* C) {
    camera_move_vertical(C, CAMERA_SPEED);
}
void camera_move_down(struct Camera* C) {
    camera_move_vertical(C, -CAMERA_SPEED);
}
void camera_move_horizontal(struct Camera* C, double d, double v ) {
    if (C->slow) d *= CAMERA_SLOW_MULTIPLIER;
    camera_accelerate(C, 0, -d * sin(v));
    camera_accelerate(C, 2, -d * cos(v));
}
void camera_move_vertical(struct Camera* C, double d ) {
    if (C->slow) d *= CAMERA_SLOW_MULTIPLIER;
    camera_accelerate(C, 1, d);
}
void camera_accelerate(struct Camera* C, int i, double a) { C->vel[i] += a; }

void camera_rotate(struct Camera* C, int u, int v) {
    C->rot[0] = fmod(C->rot[0] + v*MOUSE_SENSITIVITY, M_PI*2);
    C->rot[1] = fmod(C->rot[1] + u*MOUSE_SENSITIVITY, M_PI*2);
}

void camera_frame_update(struct Camera* C, int fps) {
    camera_update_rotation(C);
    camera_update_position(C, fps);
}

void camera_update_rotation(struct Camera* C) {
    for (int i = 0; i < 3; i++) {
        C->frame_cos[i] = cos(C->rot[i]);
        C->frame_sin[i] = sin(C->rot[i]);
    }
    camera_calc_rotation_matrix(C);
    static double camera_dir_unrotated[3][1] = {{0},{0}, {1}};
    static double camera_dir_rotated[3][1];
    matrix_product(3, 3, C->rotation_matrix,
                   3, 1, camera_dir_unrotated,
                   camera_dir_rotated);
    for (int i = 0; i < 3; i++) C->direction[i] = camera_dir_rotated[i][0];
}

void camera_update_position(struct Camera* C, int fps) {
    fps++;
    for (int i = 0; i < 3; i++) {
        C->vel[i] += C->acc[i]/(fps);
        C->vel[i] *= CAMERA_VELOCITY_LOSS;
        C->pos[i] += C->vel[i]/(fps);
    }
}

