#include "previewer.h"

int main(int argc, char* args[]) {
    char* config_path = "render.cfg";
    char* output_path = "render.cfg";
    int arg_status = handle_args(argc, args, &config_path, &output_path);
    if (arg_status != CONTINUE) return arg_status;

    struct World world;
    struct Camera camera;
    if (!config_parse(config_path, &world, &camera)) return EXIT_FAILURE;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!init_sdl(&window, &renderer, camera.res)) return EXIT_FAILURE;

    run_previewer(window, renderer, &world, &camera, config_path, output_path);
    
    cleanup(window, renderer, &camera);
    return EXIT_SUCCESS;
}

void run_previewer(SDL_Window* window,
                   SDL_Renderer* renderer,
                   struct World* world,
                   struct Camera* camera,
                   const char* input,
                   const char* output) {
    bool quit = false;
    double period = 0;
    unsigned int tick = 0;
    SDL_Event event;
    SDL_PollEvent(&event);

    while (!quit)  {
        poll_events(&event, window, camera, input, output, &quit);
        handle_controls(window, camera);
        render(window, renderer, world, camera, period);

        period = (SDL_GetTicks()-tick)/1000.0;
        tick = SDL_GetTicks();
    }
}

void poll_events(SDL_Event* event,
                 SDL_Window* window,
                 struct Camera* camera,
                 const char* input,
                 const char* output,
                 bool* quit) {
    while (SDL_PollEvent(event) != 0) {
        switch (event->type) {
        /* System */
        case SDL_QUIT: /* gets exit call from OS/WM */
            *quit = true;
            break;
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                camera->res[0] = event->window.data1;
                camera->res[1] = event->window.data2;
                camera_calc_array_size(camera);
                break;
            }
            break;
        /* Controls */
        case SDL_MOUSEMOTION:
            camera_rotate(camera, event->motion.xrel, event->motion.yrel);
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                /* exit */
                *quit = true;
                break;
            case SDL_SCANCODE_LSHIFT:
                camera_set_slow(camera, true);
                break;
            case SDL_SCANCODE_TAB:
                camera_toggle_mode(camera);
                break;
            case SDL_SCANCODE_F11:
                toggle_fullscreen(window);
                break;
            case SDL_SCANCODE_RETURN:
                /* save camera to config */
                if (config_write_camera(input, output, camera)) {
                    printf("Output saved to '%s'.\n", output);
                }
                else {
                    fprintf(stderr, "previewer: failed to save config to '%s'\n",
                            output);
                }
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            if (event->key.keysym.scancode == SDL_SCANCODE_LSHIFT) {
                camera_set_slow(camera, false);
            }
            break;
        }
    }
}

void toggle_fullscreen(SDL_Window* window) {
    bool fullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
    SDL_SetWindowFullscreen(window, (SDL_bool)(!fullscreen));
    SDL_Log("Switched to %s.", (fullscreen ? "window" : "fullscreen"));
}

void handle_controls(SDL_Window* window, struct Camera* camera) {
    const Uint8* key_states = SDL_GetKeyboardState(NULL);
    if (key_states[SDL_SCANCODE_W]) { camera_move_forward(camera); }
    if (key_states[SDL_SCANCODE_A]) { camera_move_left(camera); }
    if (key_states[SDL_SCANCODE_S]) { camera_move_back(camera); }
    if (key_states[SDL_SCANCODE_D]) { camera_move_right(camera); }
    if (key_states[SDL_SCANCODE_SPACE]) { camera_move_up(camera); }
    if (key_states[SDL_SCANCODE_LCTRL]) { camera_move_down(camera); }
    if (key_states[SDL_SCANCODE_R]) { camera_decrease_fov(camera); }
    if (key_states[SDL_SCANCODE_F]) { camera_increase_fov(camera); }
    if (key_states[SDL_SCANCODE_X]) { camera_stop(camera); }
}

void render(SDL_Window* window,
            SDL_Renderer* renderer,
            struct World* world,
            struct Camera* camera,
            double period) {
    clear_render_screen(renderer, 0xFF, 0xFF, 0xFF);
    camera_frame_update(camera, period);
    painters_render_scene(renderer, world, camera);
    update_surface(renderer, window);
}

void draw_bitmap(SDL_Renderer* renderer, struct Bitmap* bitmap) {
    for (int y = 0; y < bitmap->height; y++) {
        for (int x = 0; x < bitmap->width; x++) {
            struct Pixel* p = bitmap_get_pixel(bitmap, x, y);
            SDL_SetRenderDrawColor(renderer, p->red, p->green, p->blue, 0xFF);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

void clear_render_screen(SDL_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
    SDL_RenderClear(renderer);
}

void update_surface(SDL_Renderer* renderer, SDL_Window* window) {
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

int handle_args(int argc, char* args[], char** config_path, char** output_path) {
    bool success = true;
    for (int i = 1; i < argc; i++) {
        char* arg = args[i];
        if (arg[0] == '-') {
            char option = arg[1];
            switch (option) {
            case 'c':
                *config_path = args[++i];
                break;
            case 'o':
                *output_path = args[++i];
                break;
            case 'h':
                printf("Usage: preview [options..]\n\n"
    "Options:\n"
    "\t-c <config_path>    specify input config file [default: render.cfg]\n"
    "\þ-o <output_path>    specify output config file [default: render.cfg]\n"
    "\t-h                  print this help screen\n\n"
    "Controls:\n"
    "\tW, A, S, D          accelerate camera forward, left, backward or right\n"
    "\t<space>, <lctrl>    accelerate camera upward or downward\n"
    "\tR, F                decrease or increase field of view\n"
    "\tX                   stop camera\n"
    "\t<tab>               toggle between rendering vertices and surfaces\n"
    "\t<enter>             save current settings to output config file\n"
    "\tF11                 toggle between windowed and fullscreen mode\n"
    );
                return EXIT_SUCCESS;
                break;
            default:
                fprintf(stderr, "previewer: invalid option -- '%c'\n", option);
                success = false;
            }
        }
        else {
            fprintf(stderr, "previewer: invalid argument -- '%s'\n", arg);
            success = false;
        }
    }
    if (success) return CONTINUE;
    else {
        printf("Try 'preview -h' for more information");
        return EXIT_FAILURE;
    }
}

bool init_sdl(SDL_Window** window, SDL_Renderer** renderer, int res[]) {
    *window = init_window(res);
    if (*window == NULL) {
        fprintf(stderr, "previewer: could not create window");
        return false;
    }
    *renderer = init_renderer(*window, res);
    if (*renderer == NULL) {
        fprintf(stderr, "previewer: could not create renderer");
        return false;
    }
    return true;
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

    return renderer;
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer, struct Camera* camera) {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_Quit();
}
