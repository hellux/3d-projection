#include <stdio.h>
#include <stdlib.h>

/* bool type */
#include <stdbool.h>

#include "vector.h"
#include "matrix.h"
#include "object.h"

#define OBJECTS_START_BUFFER 10
#define OBJ_LINE_BUFFER_SIZE 128

void PRJ_add_object(char* obj_file_path,
                    double pos[],
                    double rot[],
                    int color[],
                    double brightness,
                    double reflectiveness);
bool _calc_ray_collision(double S[], double V[], struct Triangle T, double P[]);
void _calc_collision_point_plane(double S[], double V[], 
                                 struct Triangle T, double P[]);
bool _calc_point_in_triangle(double P[], struct Triangle T);
