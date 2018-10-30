#include "projector.h"

void painters_render_scene(SDL_Renderer* renderer,
                           struct World* W,
                           struct Camera* C) {
    for (int o = 0; o < W->object_count; o++) {
        for (int v = 0; v < W->objects[o].vertc; v++) {
            W->objects[o].verts_in_front[v] = camera_faces_point(C, W->objects[o].verts[v]);
            transform_point(C,
                            W->objects[o].verts[v],
                            W->objects[o].verts_2d[v]);
        }
    }

    if (C->mode == VERTEX_RENDER) render_scene_vertex(renderer, W, C);
    else if (C->mode == SURFACE_RENDER) render_scene_surface(renderer, W, C);
}

void render_scene_vertex(SDL_Renderer* renderer, struct World* W, struct Camera* C) {
    for (int o = 0; o < W->object_count; o++) {
        SDL_SetRenderDrawColor(renderer,
                               W->objects[o].color[0],
                               W->objects[o].color[1],
                               W->objects[o].color[2],
                               0xFF);
        for (int v = 0; v < W->objects[o].vertc; v++) {
            if (W->objects[o].verts_in_front[v]) {
                SDL_RenderDrawPoint(renderer,
                                    (int)W->objects[o].verts_2d[v][0],
                                    (int)W->objects[o].verts_2d[v][1]);
            }
        }
    }
}

void render_scene_surface(SDL_Renderer* renderer, struct World* W, struct Camera* C) {
    for (int o = 0; o < W->object_count; o++) {
        for (int t = 0; t < W->objects[o].tric; t++) {
            triangle_calc_camera_depth(&W->objects[o].tris[t], C);
        }
    }
    /* sort triangle array farthest to closest */
    qsort(W->triangles,
          W->triangle_count,
          sizeof(struct Triangle*),
          triangle_compare_depth);

    for (int t = 0; t < W->triangle_count; t++) {
        render_triangle(renderer, C, W->triangles[t]);
    }
}

void render_triangle(SDL_Renderer* renderer, struct Camera* C, struct Triangle* T) {
    bool in_front = false;
    for (int v = 0; v < 3; v++) {
        if (T->object->verts_in_front[T->indices[v]]) {
            in_front = true;
            break;
        }
    }
    if (in_front) {
        SDL_SetRenderDrawColor(renderer, T->color[0],
                                         T->color[1],
                                         T->color[2],
                                         0xFF);
        render_fill_triangle(renderer,
                             T->object->verts_2d[T->indices[0]],
                             T->object->verts_2d[T->indices[1]],
                             T->object->verts_2d[T->indices[2]]);
                      
    }
}

/* draw pixels inside triangle, check with barycentric coords
 *  barycentric orientation of x, y for vertices v1, v2:
 *
 *    w_o(v1,v2,x,y) = | v2[0]-v1[0] x-v1[0] | = (v2[0]-v1[0])*(y-v1[1]) -
 *                     | v2[1]-v1[1] y-v1[1] |   (x-v1[0])*(v2[1]-v1[1])
 *
 *  x12 = v2[0]-v1[0]
 *  y12 = v2[1]-v1[0]
 *   =>
 *  w_0(v1,v2,x,  y  ) = (y-v1[1])*x12 - (x-v1[0])*y12 
 *  w_0(v1,v2,x+1,y  ) = (y-v1[1])*x12 - (x-v1[0])*y12 - y12
 *  w_0(v1,v2,x  ,y+1) = (y-v1[1])*x12 - (x-v1[0])*y12 + x12
 */
void render_fill_triangle(SDL_Renderer* renderer,
                          double v1[], double v2[], double v3[]) {
    int bbox_xmin = (int)(fmin(fmin(v1[0], v2[0]), v3[0])+0.5);
    int bbox_xmax = (int)(fmax(fmax(v1[0], v2[0]), v3[0])+0.5);
    int bbox_ymin = (int)(fmin(fmin(v1[1], v2[1]), v3[1])+0.5);
    int bbox_ymax = (int)(fmax(fmax(v1[1], v2[1]), v3[1])+0.5);
    int columns = bbox_xmax-bbox_xmin+1;

    double x12 = v2[0]-v1[0], y12 = v2[1]-v1[1];
    double x23 = v3[0]-v2[0], y23 = v3[1]-v2[1];
    double x31 = v1[0]-v3[0], y31 = v1[1]-v3[1];

    double w[3];
    w[0] = x23*(bbox_ymin-v2[1])-(bbox_xmin-v2[0])*y23;
    w[1] = x31*(bbox_ymin-v3[1])-(bbox_xmin-v3[0])*y31;
    w[2] = x12*(bbox_ymin-v1[1])-(bbox_xmin-v1[0])*y12;

    for (int y = bbox_ymin; y <= bbox_ymax; y++) {
        for (int x = bbox_xmin; x <= bbox_xmax; x++) {
            if (w[0] > 0 && w[1] > 0 && w[2] > 0)
                SDL_RenderDrawPoint(renderer, x, y);
            w[0] -= y23;
            w[1] -= y31;
            w[2] -= y12;
        }
        // go back to first column, up one row
        w[0] += x23 + y23*columns;
        w[1] += x31 + y31*columns;
        w[2] += x12 + y12*columns;
    }
}

void transform_point(struct Camera* C, double P[], double point_2d[]) {
    double D[3];
    vector_subtract(P, C->pos, D);
    vector_transform(C->rotation_matrix_inv, D);

    point_2d[0] = C->res[0] / 2 + (D[0]*C->res[0]) / (D[2]*C->array_width) *
            C->focal_length;
    point_2d[1] = C->res[1] / 2 - (D[1]*C->res[1]) / (D[2]*C->array_height) *
            C->focal_length;
}
