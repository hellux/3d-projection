#include <SDL2/SDL.h>

#include "projector.hpp"

#define WINDOW_TITLE "Preview"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

SDL_Window* init_window(int resolution[]);
SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]);
void main_loop(SDL_Window* window, SDL_Renderer* renderer, int resolution[]);
void poll_events(SDL_Event* event, SDL_Window* window,
                 int resolution[], bool *quit);
void handle_keys(SDL_Window* window, bool *quit);
void render(SDL_Window* window, SDL_Renderer* renderer,
            int resolution[], int *fps);
void cleanup(SDL_Window* window, SDL_Renderer* renderer);
