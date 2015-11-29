#include "previewer.h"

int main(int argc, char* args[]) {
	/* handle config and arguments */
	struct World world;
	struct Camera camera;
	char* config_path = "preview.cfg";
	char* output_path = "render.cfg";

    for (int i = 1; i < argc; i++) {
		char* arg = args[i];
		if (arg[0] == '-') {
			char option = arg[1];
			switch (option) {
			case 'c':
				config_path = args[++i];
                break;
			case 'o':
				output_path = args[++i];
                break;
			default:
				fprintf(stderr, "previewer: invalid option -- '%c'\n", option);
            }
        }
        else {
            fprintf(stderr, "renderer: invalid argument -- '%s'\n", arg);
            return EXIT_FAILURE;
        }
    }
    if (!config_parse(config_path, &world, &camera)) {
        return EXIT_FAILURE;
    }

	/* initiate SDL */
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    window = init_window(camera.res);
	if (window == NULL) {
		fprintf(stderr, "previewer: could not create window");
		return EXIT_FAILURE;
	}
    renderer = init_renderer(window, camera.res);
	if (renderer == NULL) {
		fprintf(stderr, "previewer: could not create renderer");
		return EXIT_FAILURE;
	}
    
	/* run previewer */
    run_previewer(window, renderer, &world, &camera, config_path, output_path);
    
	/* clean up */
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
    int fps = 1000;
    Uint64 tick = 0;
    SDL_Event event;
    SDL_PollEvent(&event);

    while (!quit)  {
        poll_events(&event, window, camera, input, output, &quit);
        handle_controls(window, camera);
        render(window, renderer, world, camera, &fps);

        fps = SDL_GetPerformanceFrequency()/(SDL_GetPerformanceCounter()-tick);
        tick = SDL_GetPerformanceCounter();
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
            case SDL_SCANCODE_ESCAPE:
                /* exit */
                *quit = true;
                break;
            case SDL_SCANCODE_LSHIFT:
                camera_set_slow(camera, true);
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
}

void render(SDL_Window* window,
            SDL_Renderer* renderer,
            struct World* world,
            struct Camera* camera,
            int *fps) {
    /* clear screen */
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    /* render objects */
    camera_frame_update(camera, *fps);
    PRJ_render(renderer, world, camera);

    /* update the surface */
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera) {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, cfg_path)) {
        fprintf(stderr, "previewer: %s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        fprintf(stderr, "previewer: could not parse config file '%s'\n", cfg_path);
        return false;
    }

    bool valid_config = (config_add_camera(cfg, camera) &&
                         config_add_objects(cfg, world));
    config_destroy(&cfg);
    /* return if the camera and at least one object was loaded successfully */
    return valid_config;
}

bool config_add_camera(config_t cfg, struct Camera* camera) {
    config_setting_t *stn_cam = config_lookup(&cfg, "camera");
    if (stn_cam == NULL) {
        fprintf(stderr, "previewer: camera settings not found\n");
        return false;
    }

    double pos[] = {0, 0, 0}, rot[] = {0, 0, 0};
    int res[2];
    double fov, focal_length;

    /* parse position, rotation and resolution */
    config_setting_t* stn_pos = config_setting_lookup(stn_cam, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) {
            pos[i] = config_setting_get_float_elem(stn_pos, i);
        }
    }
    config_setting_t* stn_rot = config_setting_lookup(stn_cam, "rot");
    if (stn_rot != NULL) {
        for (int i = 0; i < 3; i++) {
            rot[i] = config_setting_get_float_elem(stn_rot, i);
        }
    }
    config_setting_t* stn_res = config_setting_lookup(stn_cam, "res");
    if (stn_res != NULL) {
        for (int i = 0; i < 2; i++) {
            res[i] = config_setting_get_int_elem(stn_res, i);
        }
    }

    /* parse field of view and focal length */
    config_setting_lookup_float(stn_cam, "fov", &fov);
    config_setting_lookup_float(stn_cam, "focal_length", &focal_length);

    /* create camera */
    struct Camera* c = camera_create(pos, rot, res, fov, focal_length);
    if (c == NULL) {
        return false;
    }
    else {
        *camera = *c;
        return true;
    }
}

bool config_add_objects(config_t cfg, struct World* world) {
    config_setting_t *object_list;
    bool has_objects = false;

    /* create world struct */
    *world = *world_create();

    object_list = config_lookup(&cfg, "objects");
    if (object_list != NULL) {
        int count = config_setting_length(object_list);
        for (int i = 0; i < count; i++) {
            config_setting_t* object = config_setting_get_elem(object_list, i);
            if (object != NULL) {
                if (config_add_object(object, world)) has_objects = true;
            }
        }
    }
    return has_objects; /* at least one object loaded successfully */
}

bool config_add_object(config_setting_t* object, struct World* world) {
    const char* obj_file_path;
    double pos[3];
    int light;

    if (!config_setting_lookup_string(object, "path", &obj_file_path)) {
        fprintf(stderr, "previewer: no path to model in object section\n");
        return false;
    }

    config_setting_t* stn_pos = config_setting_lookup(object, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) {
            pos[i] = config_setting_get_float_elem(stn_pos, i);
        }
    }
    config_setting_lookup_bool(object, "light", &light);

    /* return if object was successfully added to world struct */
    return world_add_object(world, obj_file_path, pos, (bool)light);
}

bool config_write_camera(const char* in_path,
                         const char* out_path,
                         struct Camera* camera) {
    config_t cfg;
    config_init(&cfg);
        
    if (!config_read_file(&cfg, in_path)) {
        fprintf(stderr, "previewer: %s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        fprintf(stderr, "previewer: could not parse config file '%s'\n", in_path);
        return false;
    }

    config_setting_t* stn_cam = config_lookup(&cfg, "camera");
    if (stn_cam == NULL) {
        fprintf(stderr, "renderer: camera settings not found\n");
        return false;
    }

    config_setting_t* stn_pos = config_setting_lookup(stn_cam, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) {
            config_setting_set_float_elem(stn_pos, i, camera->pos[i]);
        }
    }
    config_setting_t* stn_rot = config_setting_lookup(stn_cam, "rot");
    if (stn_rot != NULL) {
        for (int i = 0; i < 3; i++) {
            config_setting_set_float_elem(stn_rot, i, camera->rot[i]);
        }
    }
    
    config_setting_set_float(config_setting_lookup(stn_cam, "fov"),
                             camera->fov);
    config_setting_set_float(config_setting_lookup(stn_cam, "focal_length"),
                             camera->dims[2]);

    if (!config_write_file(&cfg, out_path)) {
        fprintf(stderr, "previewer: could not write config file to '%s'", out_path);
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
