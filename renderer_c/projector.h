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

#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "object.h"
#include "image.h"

#define OBJECTS_START_BUFFER 3
#define OBJ_LINE_BUFFER_SIZE 128

void PRJ_add_object(char* obj_file_path,
                    double pos[],
                    uint8_t color[],
                    double brightness,
                    double reflectiveness);
void PRJ_render(double pos[], double rot[], int res[],
                double fov, double focal_length);

static void rotate_vertex_x(double vertex[], double point[], double ang);
static void rotate_vertex_y(double vertex[], double point[], double ang);
static void calc_pixel_color(struct Camera C, int row, int col, uint8_t color[]);
static void calc_camera_vector(struct Camera C, int row, int col, double V[]);
static bool calc_ray_collision(double S[], double V[], 
                               struct Triangle T, double P[]);
static void calc_collision_point_plane(double S[], double V[],
                                       struct Triangle T, double P[]);
static bool calc_point_in_triangle(double P[], struct Triangle T);
static void print_progress_bar(double progress);
