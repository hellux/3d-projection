#ifndef projector_h
#define projector_h

#include <stdio.h>
#include <stdlib.h>

/* log and trig */
#include <math.h>

/* get time for eta */
#include <time.h>

/* uint8_t type */
#include <stdint.h>

/* bool type */
#include <stdbool.h>

#include "../common/world.h"
#include "../common/object.h"
#include "../common/camera.h"
#include "../common/vector.h"
#include "../common/matrix.h"
#include "../common/image.h"

#define OBJ_LINE_BUFFER_SIZE 128

struct Bitmap* projector_render(struct World W, struct Camera C);
void rotate_vertex_x(double vertex[], double point[], double ang);
void rotate_vertex_y(double vertex[], double point[], double ang);
void calc_pixel_color(struct World W,
                      struct Camera C,
                      int row, int col,
                      uint8_t color[]);
void calc_camera_vector(struct Camera C, int row, int col, double V[]);
double calc_ray_triangle_collision(double S[], double V[], struct Triangle T);
double calc_ray_plane_collision(double S[], double V[], struct Triangle T);
bool calc_point_in_triangle(double P[], struct Triangle T);
void calc_vector_plane_reflection(double V[], double N[], double R[]);
void print_progress_bar(double progress);

#endif
