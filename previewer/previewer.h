#ifndef previewer_h
#define previewer_h

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "projector.h"
#include "camera.h"
#include "world.h"

#define WINDOW_TITLE "Preview"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

void run_previewer(SDL_Window* window,
                   SDL_Renderer* renderer,
                   struct World* world,
                   struct Camera* camera);
void poll_events(SDL_Event* event,
                        SDL_Window* window,
                        struct Camera* camera,bool *quit);
void handle_keys(SDL_Window* window, struct Camera* camera, bool *quit);
void render(SDL_Window* window,
            SDL_Renderer* renderer,
            struct World* world,
            struct Camera* camera,
            int *fps);
SDL_Window* init_window(int resolution[]);
SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]);
void cleanup(SDL_Window* window,
             SDL_Renderer* renderer,
             struct Camera* camera);

#endif
