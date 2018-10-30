#ifndef projector_h
#define projector_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /* uint8_t type for colors */
#include <limits.h> /* INT_MAX, INT_MIN */
#include <math.h> /* abs, trig */
#include <SDL2/SDL.h>

#include "../common/world.h"
#include "../common/camera.h"
#include "../common/object.h"
#include "../common/triangle.h"
#include "../common/vector.h"
#include "../common/image.h"

void painters_render_scene(SDL_Renderer* renderer,
                           struct World* W,
                           struct Camera* C);
void render_scene_vertex(SDL_Renderer* renderer, struct World* W, struct Camera* C);
void render_scene_surface(SDL_Renderer* renderer, struct World* W, struct Camera* C);
void render_vertex(SDL_Renderer* renderer, struct Camera* C, double vertex[]);
void render_triangle(SDL_Renderer* renderer, struct Camera* C,  struct Triangle* T);
void render_fill_triangle(SDL_Renderer* renderer,
                     double v1[], double v2[], double v3[]);
void transform_point(struct Camera* C, double P[], double point_2d[]);

#endif
