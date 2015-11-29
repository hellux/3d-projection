#ifndef camera_h
#define camera_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CAMERA_SPEED 1
#define CAMERA_VELOCITY_LOSS 0.9
#define CAMERA_SLOW_MULTIPLIER 0.1
#define MOUSE_SENSITIVITY 0.0001

struct Camera {
    double pos[3];
    double vel[3];
    double acc[3];
    double rot[2];
    double dims[3];
    double fov;
    int res[2];

    bool slow;
    double cos[2];
    double sin[2];
};

struct Camera* camera_create(double pos[], double rot[], int res[], 
                             double fov, double focal_lengt);
void camera_calc_dimensions(struct Camera* C);
void camera_set_slow(struct Camera* C, bool s);
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
void camera_frame_update(struct Camera* C, int fps);
void camera_update_rotation(struct Camera* C);
void camera_update_position(struct Camera* C, int fps);

#endif
