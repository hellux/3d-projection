#ifndef projector_h
#define projector_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "world.h"
#include "camera.h"
#include "object.h"

void PRJ_render(SDL_Renderer* renderer, struct World* W, struct Camera* C);
void render_object(SDL_Renderer* renderer, struct Camera* C, struct Object* O);
void render_point(SDL_Renderer* renderer, struct Camera* C, double vertex[]);
void transform_point(int point_2d[], struct Camera* C, double P[]);

#endif
