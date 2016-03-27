#ifndef camera_h
#define camera_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../common/vector.h"
#include "../common/matrix.h"

#define VERTEX_RENDER 0
#define SURFACE_RENDER 1

#define CAMERA_SPEED 1
#define CAMERA_VELOCITY_LOSS 0.9
#define CAMERA_SLOW_MULTIPLIER 0.1
#define CAMERA_FOV_STEP_FACTOR 1.001
#define MOUSE_SENSITIVITY 0.0001

struct Camera {
    double pos[3];
    double rot[3];
    double vel[3];
    double acc[3];
    double array_width, array_height;
    double focal_length;
    double fov;
    int res[2];
    double direction[3]; /* plane normal */
    double signed_distance;
    double light[3];
    double pixel_size;
    double rotation_matrix[3][3];
    double rotation_matrix_inv[3][3];

    int mode;
    bool slow;
    double frame_cos[3];
    double frame_sin[3];
};

struct Camera* camera_create(double pos[], double rot[], int res[], 
                             double fov, double focal_length, double light[]);
void camera_set_position(struct Camera* C, double pos[], double rot[]);
void camera_set_light(struct Camera* C, double light[]);
bool camera_set_resolution(struct Camera* C, int res[]);
bool camera_set_focal_length(struct Camera* C, double focal_length);
bool camera_set_fov(struct Camera* C, double fov);
bool camera_set_mode(struct Camera* C, int mode);
void camera_calc_array_size(struct Camera* C);
void camera_calc_rotation_matrix(struct Camera* C);
void camera_calc_pixel_direction(struct Camera C, int row, int col, double V[]);
bool camera_faces_point(struct Camera* C, double point[]);
void camera_set_slow(struct Camera* C, bool s);
void camera_increase_fov(struct Camera* C);
void camera_decrease_fov(struct Camera* C);
void camera_change_fov(struct Camera* C, bool increase);
void camera_stop(struct Camera* C);
void camera_move_forward(struct Camera* C);
void camera_move_back(struct Camera* C);
void camera_move_right(struct Camera* C);
void camera_move_left(struct Camera* C);
void camera_move_up(struct Camera* C);
void camera_move_down(struct Camera* C);
void camera_move_horizontal(struct Camera* C, double d, double v );
void camera_move_vertical(struct Camera* C, double d );
void camera_accelerate(struct Camera* C, int i, double a);
void camera_rotate(struct Camera* C, int u, int v);
void camera_frame_update(struct Camera* C, double period);
void camera_update_rotation(struct Camera* C);
void camera_update_position(struct Camera* C, double period);
int camera_toggle_mode(struct Camera* C);

#endif
