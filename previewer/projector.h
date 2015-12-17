#ifndef projector_h
#define projector_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "../common/world.h"
#include "../common/camera.h"
#include "../common/object.h"
#include "../common/triangle.h"
#include "../common/vector.h"

void render_scene(SDL_Renderer* renderer, struct World* W, struct Camera* C);
void render_object(SDL_Renderer* renderer, struct Camera* C, struct Object* O);
void render_triangle(SDL_Renderer* renderer,
                     struct Camera* C,
                     struct Triangle* T);
int triangle_compare_depth(const void* t1, const void* t2);
int vertex_compare_y(const void* v1, const void *v2);
void render_fill_triangle(SDL_Renderer* renderer, 
                           double v1[], double v2[], double v3[]);
void render_fill_top_flat_triangle(SDL_Renderer* renderer,
                                   double v1[], double v2[], double v3[]);
void render_fill_bottom_flat_triangle(SDL_Renderer* renderer,
                                      double v1[], double v2[], double v3[]);
void render_hori_line(SDL_Renderer* renderer, int x1, int x2, int y);
void transform_point(struct Camera* C, double P[], double point_2d[]);

#endif
