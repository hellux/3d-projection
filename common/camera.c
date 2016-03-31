#include "camera.h"

struct Camera* camera_create(double pos[], double rot[], int res[],
                             double fov, double focal_length, double light[]) {
    bool success = true;
    struct Camera* C = malloc(sizeof(struct Camera));
    camera_set_slow(C, false);
    camera_set_position(C, pos, rot);
    camera_set_light(C, light);
    if (!camera_set_resolution(C, res)) success = false;
    if (!camera_set_focal_length(C, focal_length)) success = false;
    if (!camera_set_fov(C, fov)) success = false;
    if (!camera_set_mode(C, VERTEX_RENDER)) success = false;
    camera_calc_array_size(C);
    camera_update_rotation(C);

    if (success) return C;
    else return NULL;
}

void camera_set_position(struct Camera* C, double pos[], double rot[]) {
    for (int i = 0; i < 3; i++) {
        C->pos[i] = pos[i];
        C->rot[i] = fmod(rot[i], M_PI*2);
        C->acc[i] = 0;
        C->vel[i] = 0;
    }
}

void camera_set_light(struct Camera* C, double light[]) {
    for (int i = 0; i < 3; i++) {
        C->light[i] = light[i];
    }
}

bool camera_set_resolution(struct Camera* C, int res[]) {
    for (int i = 0; i < 2; i++) {
        if (res[i] > 0) C->res[i] = res[i];
        else {
            fprintf(stderr, "previewer: invalid resolution -- %dx%d px\n",
                    res[0], res[1]);
            return false;
        }
    }
    return true;
}

bool camera_set_focal_length(struct Camera* C, double focal_length) {
    if (focal_length > 0) C->focal_length = focal_length;
    else {
        fprintf(stderr, "camera: invalid focal length -- %f m\n", focal_length);
        return false;
    }
    return true;
}

bool camera_set_fov(struct Camera* C, double fov) {
    if (fov > 0 && fov < M_PI) C->fov = fov;
    else {
        fprintf(stderr, "camera: invalid field of view -- %f radians\n", fov);
        return false;
    }
    return true;
}

bool camera_set_mode(struct Camera* C, int mode) {
    if (mode >= 0 && mode < 2) C->mode = mode;
    else {
        fprintf(stderr, "camera: invalid mode -- %d\n", mode);
        return false;
    }
    return true;
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
    C->pixel_size = C->array_width / C->res[0];
}

void camera_calc_rotation_matrix(struct Camera* C) {
    double rot_x[3][3], rot_y[3][3], rot_z[3][3];

    matrix_rotation_x(C->frame_cos[0], C->frame_sin[0], rot_x);
    matrix_rotation_y(C->frame_cos[1], C->frame_sin[1], rot_y);
    matrix_rotation_z(C->frame_cos[2], C->frame_sin[2], rot_z);

    /* combined rotation matrix
     * R = Rz(c) * Ry(b) * Rx(a) */
    double rot_z_rot_y[3][3];
    matrix_product(3, 3, rot_z,
                   3, 3, rot_y,
                   rot_z_rot_y);
    matrix_product(3, 3, rot_z_rot_y,
                   3, 3, rot_x,
                   C->rotation_matrix);
    matrix_inverse_3x3(C->rotation_matrix, C->rotation_matrix_inv);
}

void camera_calc_pixel_direction(struct Camera C, int row, int col, double V[]) {
    /*
     *    focal length         |---- width ----|
     *  |---------------|  c ^         ^ y        [m]
     *                       |  _______|_______         _
     *  ^ y             |    H |_|_|_|_|_|_|_|_|        |
     *  |  -z           |    : |_|_|_|_|_|_|_|_|   x    |
     *  C -->  -  -  -  |    2 |_|_|_|_C_|_|_|_|--->  height
     *                  |    1 |_|_|_|_|_|_|_|_|        |
     *                  |    0 |_|_|_|_|_|_|_|_|        |
     *                          0 1 2 3 4 ... W -->     -
     *                                            r
     * C: camera
     * W, H: resolution width, height
     * r, c: row, column
     *
     * V0 = (x, y, z)
     * x = -width  / 2 + (col+0.5) * pixel_size
     * y = -height / 2 + (row+0.5) * pixel_size
     * z = -focal_length
     *
     * V = (x', y', z')
     *
     *     |x'|            |x|
     * V = |y'| = R*V0 = R*|y|
     *     |z'|            |z| */

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

bool camera_faces_point(struct Camera* C, double point[]) {
    double sign = dot_product(C->direction, point);
    return sign + C->signed_distance > 0;
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

void camera_stop(struct Camera* C) {
    for (int i = 0; i < 3; i++) {
        C->vel[i] = 0;
        C->acc[i] = 0;
    }
}

void camera_move_forward(struct Camera* C) { 
    double acceleration[3];
    vector_multiply(CAMERA_SPEED, C->direction, acceleration);
    vector_add(C->vel, acceleration, C->vel);
}
void camera_move_back(struct Camera* C) {
    double acceleration[3];
    vector_multiply(CAMERA_SPEED, C->direction, acceleration);
    vector_subtract(C->vel, acceleration, C->vel);
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
    camera_accelerate_axis(C, 0, -d * sin(v));
    camera_accelerate_axis(C, 2, -d * cos(v));
}
void camera_move_vertical(struct Camera* C, double d ) {
    if (C->slow) d *= CAMERA_SLOW_MULTIPLIER;
    camera_accelerate_axis(C, 1, d);
}
void camera_accelerate_axis(struct Camera* C, int i, double a) { C->vel[i] += a; }

void camera_rotate(struct Camera* C, int x, int y) {
    C->rot[0] = fmod(C->rot[0] + y*MOUSE_SENSITIVITY, M_PI*2);
    C->rot[1] = fmod(C->rot[1] + x*MOUSE_SENSITIVITY, M_PI*2);
}

void camera_frame_update(struct Camera* C, double period) {
    camera_update_rotation(C);
    camera_update_position(C, period);
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
    C->signed_distance = -dot_product(C->direction, C->pos);
}

void camera_update_position(struct Camera* C, double period) {
    for (int i = 0; i < 3; i++) {
        C->vel[i] += C->acc[i]*period;
        C->vel[i] *= CAMERA_VELOCITY_LOSS;
        C->pos[i] += C->vel[i]*period;
    }
}

int camera_toggle_mode(struct Camera* C) {
    C->mode = !C->mode;
    return C->mode;
}
