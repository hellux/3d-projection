#include "camera.h"

struct Camera* camera_create(double pos[], double rot[], int res[],
                             double fov, double focal_length) {
    struct Camera* C = malloc(sizeof(struct Camera));
    
    for (int i = 0; i < 3; i++) C->pos[i] = pos[i];
    for (int i = 0; i < 2; i++) {
        C->rot[i] = fmod(rot[i], M_PI/2);
        if (res[i] > 0) C->res[i] = res[i];
        else {
            fprintf(stderr, "previewer: invalid resolution -- %dx%dpx\n",
                    res[0], res[1]);
            return NULL;
        }
    }

    if (focal_length > 0) C->dims[2] = focal_length;
    else {
        fprintf(stderr, "previewer: invalid focal length -- %f m\n", focal_length);
        return NULL;
    }
    if (fov > 0 && fov < M_PI) C->fov = fov;
    else {
        fprintf(stderr, "previewer: invalid field of view");
        return NULL;
    }

    camera_calc_dimensions(C);

    return C;
}

void camera_calc_dimensions(struct Camera* C) {
    C->dims[0] = C->dims[2] * tan(C->fov/2) * 2;
    C->dims[1] = C->dims[0] * C->res[1] / C->res[0];
}

void camera_set_slow(struct Camera* C, bool s) { C->slow = s; }

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
    for (int i = 0; i < 2; i++) {
        C->cos[i] = cos(C->rot[i]);
        C->sin[i] = sin(C->rot[i]);
    }
}

void camera_update_position(struct Camera* C, int fps) {
    fps++;
    for (int i = 0; i < 3; i++) {
        C->vel[i] += C->acc[i]/(fps);
        C->vel[i] *= CAMERA_VELOCITY_LOSS;
        C->pos[i] += C->vel[i]/(fps);
    }
}

