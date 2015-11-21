#include "renderer.h"

int main(int argc, char* args) {
    double pos[] = {0, 0, 0};
    double rot[] = {0, 0, 0};
    int col[] = {0, 0, 0};

    PRJ_add_object("../data/911.obj",
                   pos, rot, col, 0, 0.5);
    return EXIT_SUCCESS;
}

