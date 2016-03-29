#include "renderer.h"

int main(int argc, char* args[]) {
    struct World world;
    struct Camera camera;
    struct Bitmap bitmap;
    char* config_path = "render.cfg";
    char* output_path = "render.png";
	int arg_status = handle_args(argc, args, &config_path, &output_path);
	if (arg_status != CONTINUE) return arg_status;

    if (!config_parse(config_path, &world, &camera)) return EXIT_FAILURE;
    bitmap = *projector_render(world, camera);

    return bitmap_save_png(&bitmap, output_path) ? EXIT_SUCCESS : EXIT_FAILURE;
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
                printf("Usage: render [options..]\n"
    "Options:\n"
    "  -c <config_path>    specify input config file [default: render.cfg]\n"
    "  -o <output_path>    specify output config file [default: render.cfg]\n"
    "  -h                  print this help screen\n");
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
        printf("Try 'render -h' for more information");
        return EXIT_FAILURE;
    }
}

