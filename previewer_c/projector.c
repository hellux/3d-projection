#include "projector.h"

void PRJ_render(SDL_Renderer* renderer, struct World* W, struct Camera* C) {
    for (int obj = 0; obj < W->object_count; obj++) {
        PRJ_render_object(renderer, C, &W->objects[obj]);
    }
}

static void PRJ_render_object(SDL_Renderer* renderer,
                              struct Camera* C,
                              struct Object* O) {
    SDL_SetRenderDrawColor(renderer, O->color[0], O->color[1], O->color[2], 0xFF);
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
