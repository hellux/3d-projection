#include "renderer.h"

int main(int argc, char* args[]) {
    struct World world;
    struct Camera camera;
    struct Bitmap bitmap;
    char* config_path = "render.cfg";
    char* output_path = "render.png";

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
                fprintf(stderr, "renderer: invalid option -- '%c'\n", option);
            }
        }
        else {
            fprintf(stderr, "renderer: invalid argument -- '%s'\n", args[i]);
            return EXIT_FAILURE;
        }
    }
    if (!config_parse(config_path, &world, &camera)) return EXIT_FAILURE;
    
    bitmap = *projector_render(world, camera);
    
    return bitmap_save_png(&bitmap, output_path) ? EXIT_SUCCESS : EXIT_FAILURE;
}
