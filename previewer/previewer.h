#ifndef previewer_h
#define previewer_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "projector.h"
#include "../common/config.h"
#include "../common/camera.h"
#include "../common/world.h"

#define WINDOW_TITLE "Preview"
#define CONTINUE 2

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
            double fps);
void clear_render_screen(SDL_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void update_surface(SDL_Renderer* renderer, SDL_Window* window);
int handle_args(int argc, char* args[], char** config_path, char** output_path);
bool init_sdl(SDL_Window** window, SDL_Renderer** renderer, int res[]);
SDL_Window* init_window(int resolution[]);
SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]);
void cleanup(SDL_Window* window,
             SDL_Renderer* renderer,
             struct Camera* camera);
#endif
