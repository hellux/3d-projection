#ifndef projector_h
#define projector_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "camera.h"
#include "object.h"

#define OBJECTS_START_BUFFER 3

void PRJ_add_object(char* obj_file_path, double pos[], int type);
void PRJ_render(SDL_Renderer* renderer, struct Camera* C);
static void PRJ_render_object(SDL_Renderer* renderer,
                              struct Camera* C,
                              struct Object* O);
static void PRJ_render_point(SDL_Renderer* renderer,
                             struct Camera* C,
                             double vertex[]);
static void PRJ_transform_point(int point_2d[], struct Camera* C, double P[]);

#endif
