#include "previewer.h"

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    int res[] = {DEFAULT_WIDTH, DEFAULT_HEIGHT};
    double pos[3];
    double rot[3];
    struct Camera* camera = camera_create(pos, rot, res, 1.7, 0.01);
    struct World* world = world_create();
    
    world_add_object(world, "../data/grid.obj", pos, 1);
    world_add_object(world, "../data/porsche-911.obj", pos, 0);

    window = init_window(res);
    renderer = init_renderer(window, res);
    
    run_previewer(window, renderer, world, camera);
    
    cleanup(window, renderer, camera);

    return EXIT_SUCCESS;
}

void run_previewer(SDL_Window* window,
                   SDL_Renderer* renderer,
                   struct World* world,
                   struct Camera* camera) {
    bool quit = false;
    int fps = 1000;
    Uint64 tick = 0;
    SDL_Event event;
    SDL_PollEvent(&event);

    while (!quit)  {
        poll_events(&event, window, camera, &quit);
        handle_keys(window, camera, &quit);
        render(window, renderer, world, camera, &fps);

        fps = SDL_GetPerformanceFrequency()/(SDL_GetPerformanceCounter()-tick);
        tick = SDL_GetPerformanceCounter();
    }
}

void poll_events(SDL_Event* event,
                 SDL_Window* window,
                 struct Camera* camera,
                 bool* quit) {
    while (SDL_PollEvent(event) != 0) {
        switch (event->type) {
        /* System */
        case SDL_QUIT:
            *quit = true;
            break;
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                camera->res[0] = event->window.data1;
                camera->res[1] = event->window.data2;
                camera_calc_dimensions(camera);
                break;
            }
            break;
        /* Controls */
        case SDL_MOUSEMOTION:
            camera_rotate(camera, event->motion.xrel, event->motion.yrel);
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_LSHIFT:
                camera_set_slow(camera, true);
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_LSHIFT:
                camera_set_slow(camera, false);
                break;
            }
            break;
        }
    }
}

void handle_keys(SDL_Window* window, struct Camera* camera, bool *quit) {
    const Uint8* key_states = SDL_GetKeyboardState(NULL);
    /* toggle fullscreen*/
    if (key_states[SDL_SCANCODE_F11]) {
        bool fullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowFullscreen(window, (SDL_bool)!fullscreen);
        SDL_Log("Switched to %s mode.", (fullscreen ? "window" : "fullscreen"));
    }
    /* exit */
    if (key_states[SDL_SCANCODE_ESCAPE]) { *quit = true; }
    /* control camera movement */
    if (key_states[SDL_SCANCODE_W]) { camera_move_forward(camera); }
    if (key_states[SDL_SCANCODE_A]) { camera_move_left(camera); }
    if (key_states[SDL_SCANCODE_S]) { camera_move_back(camera); }
    if (key_states[SDL_SCANCODE_D]) { camera_move_right(camera); }
    if (key_states[SDL_SCANCODE_SPACE]) { camera_move_up(camera); }
    if (key_states[SDL_SCANCODE_LCTRL]) { camera_move_down(camera); }
}

void render(SDL_Window* window,
            SDL_Renderer* renderer,
            struct World* world,
            struct Camera* camera,
            int *fps) {
    /* Clear screen */
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    /* Render objects */
    camera_frame_update(camera, *fps);
    PRJ_render(renderer, world, camera);

    /* Update the surface */
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

SDL_Window* init_window(int resolution[]) {
    SDL_Log("Initializing SDL %s %s\n",
            (SDL_Init(SDL_INIT_VIDEO) == 0 ? "succeeded! :D" : "failed. :o"),
            SDL_GetError());
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          resolution[0],
                                          resolution[1],
                                          SDL_WINDOW_SHOWN);
    SDL_Log("Creation of window %s %s\n",
            (window == NULL ? "failed. :(" : "succeeded, wohoo!"),
            SDL_GetError());

    return window;
}

SDL_Renderer* init_renderer(SDL_Window* window, int resolution[]) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window,
                                                1,
                                                SDL_RENDERER_ACCELERATED);
    SDL_Log("Renderer initialization %s %s",
            (renderer == NULL ? "failed, fak." : "succeeded, yay!"),
            SDL_GetError());

    return renderer;
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer, struct Camera* camera) {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_Quit();
}