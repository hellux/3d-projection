#include "projector.h"

static int objc = 0;
static int objb = OBJECTS_START_BUFFER;
static struct Object* objs;

void PRJ_add_object(char* obj_file_path, double pos[], int type) {

    /* allocate memory for array */
    if (objc == 0) {
        SDL_Log("Allocating memory for object array");
        objs = (struct Object*)malloc(sizeof(struct Object)*objb);
    }
    /* allocate more memory if the object array is about to become full */
    if (objc == objb-1) {
        SDL_Log("Allocating more memory for object array");
        objb += OBJECTS_START_BUFFER;
        objs = (struct Object*)realloc(objs, sizeof(struct Object)*objb);
    }
     
    uint8_t color[] = {255, 255, 255}; 
    SDL_Log("Creating object");
    object_create(obj_file_path, pos, color, &objs[objc]);
    objc++;
}

void PRJ_render(SDL_Renderer* renderer, struct Camera* C) {
    for (int obj = 0; obj < objc; obj++) {
        PRJ_render_object(renderer, C, &objs[obj]);
    }
}

static void PRJ_render_object(SDL_Renderer* renderer,
                              struct Camera* C,
                              struct Object* O) {
    for (int v = 0; v < O->vertexc ; v++) {
        PRJ_render_point(renderer, C, O->vertices[v]);
    }
}

static void PRJ_render_point(SDL_Renderer* renderer,
                             struct Camera* C,
                             double vertex[]) {
    int point_2d[2];
    PRJ_transform_point(point_2d,
                        C,
                        vertex);
    SDL_RenderDrawPoint(renderer, point_2d[0], point_2d[1]);
}

static void PRJ_transform_point(int point_2d[], struct Camera* C, double P[]) {
    double X = P[0] - C->pos[0];
    double Y = P[1] - C->pos[1];
    double Z = P[2] - C->pos[2];

    double dx = C->cos[1] * X - C->sin[1] * Z;
    double dy = C->sin[0] * (C->cos[1] * Z + C->sin[1] * X) + C->cos[0] * Y;
    double dz = C->cos[0] * (C->cos[1] * Z + C->sin[1] * X) - C->sin[0] * Y;

    point_2d[0] = (int) 
        (C->res[0] / 2 + (dx*C->res[0]) / (dz*C->dims[0]) * C->dims[2]);
    point_2d[1] = (int)
        (C->res[1] / 2 - (dy*C->res[1]) / (dz*C->dims[1]) * C->dims[2]);
}
