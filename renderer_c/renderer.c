#include "renderer.h"

int main(int argc, char* args) {

    /* test sub file functions */
    vector_test();
    matrix_test();

    double pos[] = {0, 0, 0};
    double posc[] = {0, 0, 0};
    double rot[] = {0, 0, 0};
    uint8_t col[] = {0xFF, 0xFF, 0xFF};
    int res[] = {320, 240};
    double fov = 1.5;
    double focal_length = 0.01;

    struct World world = *world_create();
    struct Camera camera = *camera_create(posc, rot, res, fov, focal_length);
    
    world_add_object(&world, "../data/cube_tri.obj", pos, col, 0, 0.5);
    PRJ_render(world, camera);

    return EXIT_SUCCESS;
}

