#ifndef previewer_h
#define previewer_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <libconfig.h>

#include "projector.h"
#include "camera.h"
#include "world.h"

#define WINDOW_TITLE "Preview"

void run_previewer(SDL_Window* window,
                   SDL_Renderer* renderer,
                   struct World* world,
                   struct Camera* camera,
                   const char* input,
                   const char* output);
void poll_events(SDL_Event* event,
                 SDL_Window* window,
                 struct Camera* camera,
                 const char* input,
                 const char* output,
                 bool *quit);
void toggle_fullscreen(SDL_Window* window);
void handle_controls(SDL_Window* window, struct Camera* camera);
void render(SDL_Window* window,
            SDL_Renderer* renderer,
            struct World* world,
            struct Camera* camera,
            int *fps);

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera);
bool config_add_camera(config_t cfg, struct Camera* camera);
bool config_add_objects(config_t cfg, struct World* world);
bool config_add_object(config_setting_t* object, struct World* world);
bool config_write_camera(const char* input_cfg,
                         const char* output_cfg,
                         struct Camera* camera);

SDL_Window* init_window(int resolution[]);
SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]);

void cleanup(SDL_Window* window,
             SDL_Renderer* renderer,
             struct Camera* camera);

#endif
