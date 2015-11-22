#include "previewer.hpp"

int main(int argc, char* args[]) {
    
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int resolution[] = {DEFAULT_WIDTH, DEFAULT_HEIGHT};

    window = init_window(resolution);
    renderer = init_renderer(window, resolution);
    main_loop(window, renderer, resolution);
    cleanup(window, renderer);

    return EXIT_SUCCESS;
}

SDL_Window* init_window(int resolution[]) {
    SDL_Log("Initializing SDL %s. %s\n",
            (SDL_Init(SDL_INIT_VIDEO) == 0 ? "succeeded" : "failed"),
            SDL_GetError());
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          resolution[0],
                                          resolution[1],
                                          SDL_WINDOW_SHOWN);
    SDL_Log("Creation of window %s. %s\n",
            (window == NULL ? "failed" : "succeeded"),
            SDL_GetError());

    return window;
}

SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window,
                                                1,
                                                SDL_RENDERER_ACCELERATED);
    SDL_Log("Renderer initialization %s. %s",
            (renderer == NULL ? "failed" : "succeeded"),
            SDL_GetError());
    PRJ_set_dimensions(resolution[0], resolution[1]);

    PRJ_load_obj_file("../data/grid.obj",
                      new double[3] {0, 0, 0},
                      new double[2] {0, 0});
    PRJ_load_obj_file("../data/porsche-911.obj",
                      new double[3] {0, 0, 0},
                      new double[2] {0, 0});
    return renderer;
}

void main_loop(SDL_Window* window, SDL_Renderer* renderer, int resolution[]) {
    bool quit = false;
    int fps = 1000;
    Uint64 tick = 0;
    SDL_Event event;
    SDL_PollEvent(&event);

    while (!quit)  {
        poll_events(&event, window, resolution, &quit);
        handle_keys(window, &quit);
        render(window, renderer, resolution, &fps);

        fps = SDL_GetPerformanceFrequency() /
            (SDL_GetPerformanceCounter() - tick);
        tick = SDL_GetPerformanceCounter();
    }
}

void poll_events(SDL_Event* event, SDL_Window* window,
                 int resolution[], bool* quit) {
    while (SDL_PollEvent(event) != 0) {
        switch (event->type) {
        /* System */
        case SDL_QUIT:
            *quit = true;
            break;
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                resolution[0] = event->window.data1;
                resolution[1] = event->window.data2;
                PRJ_set_dimensions(resolution[0], resolution[1]);
                break;
            }
            break;
        /* Controls */
        case SDL_MOUSEMOTION:
            PRJ_rotate(event->motion.xrel, event->motion.yrel);
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_LSHIFT:
                PRJ_set_slow(true);
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_LSHIFT:
                PRJ_set_slow(false);
                break;
            }
            break;
        }
    }
}

void handle_keys(SDL_Window* window, bool *quit) {
    const Uint8* key_states = SDL_GetKeyboardState(NULL);
    if ((key_states[SDL_SCANCODE_LALT] && key_states[SDL_SCANCODE_RETURN]) ||
         key_states[SDL_SCANCODE_F11]) {
        bool fullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
        SDL_Log("Window set to %s mode.", (fullscreen ? "window" : "fullscreen"));
        SDL_SetWindowFullscreen(window, (SDL_bool)!fullscreen);
    }
    if (key_states[SDL_SCANCODE_ESCAPE]) { *quit = true; }
    if (key_states[SDL_SCANCODE_W]) { PRJ_move_forward(); }
    if (key_states[SDL_SCANCODE_A]) { PRJ_move_left(); }
    if (key_states[SDL_SCANCODE_S]) { PRJ_move_back(); }
    if (key_states[SDL_SCANCODE_D]) { PRJ_move_right(); }
    if (key_states[SDL_SCANCODE_SPACE]) { PRJ_move_up(); }
    if (key_states[SDL_SCANCODE_LCTRL]) { PRJ_move_down(); }
}

void render(SDL_Window* window, SDL_Renderer* renderer, 
            int resolution[], int *fps) {
    /* Clear screen */
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    /* Render objects */
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    PRJ_update(*fps);
    PRJ_render(renderer, resolution);

    /* Update the surface */
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_Quit();
}
