#include "renderer.h"

int main(int argc, char* args) {
    double pos[3];
    double posc[] = {0, 123, 0};
    double rot[2];
    uint8_t col[3];
    int res[] = {400, 300};

    PRJ_add_object("../data/cube_tri.obj", pos, col, 0, 0.5);
    PRJ_render(posc, rot, res, 1.7, 0.1);

    return EXIT_SUCCESS;
}

