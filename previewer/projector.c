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
        render_fill_triangle(renderer, C->res,
                             T->object->verts_2d[T->indices[0]],
                             T->object->verts_2d[T->indices[1]],
                             T->object->verts_2d[T->indices[2]]);
                      
    }

}

void render_fill_triangle(SDL_Renderer* renderer, int res[],
                          double v1[], double v2[], double v3[]) {
    static int** contour_x;
    static int res_y = 0;
    if (res_y != res[1]) {
        res_y = res[1];
        contour_x = malloc(res[1] * sizeof(int*));
        for (int i = 0; i < res[1]; i++) {
            contour_x[i] = malloc(2 * sizeof(int));
        }
    }
    
    int y;

    for (y = 0; y < res[1]; y++)    {
        contour_x[y][0] = INT_MAX;
        contour_x[y][1] = INT_MIN;
    }

    triangle_scan_line((int)v1[0], (int)v1[1], (int)v2[0], (int)v2[1],
                       contour_x, res);
    triangle_scan_line((int)v2[0], (int)v2[1], (int)v3[0], (int)v3[1],
                       contour_x, res);
    triangle_scan_line((int)v3[0], (int)v3[1], (int)v1[0], (int)v1[1],
                       contour_x, res);

    for (y = 0; y < res[1]; y++)    {
        if (contour_x[y][1] >= contour_x[y][0]) {
            render_hori_line(renderer, contour_x[y][0], contour_x[y][1]+1, y);
        }
    }
}

void triangle_scan_line(int x1, int y1,
                        int x2, int y2,
                        int** contour_x, int res[]) {
    int sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

    sx = x2 - x1;
    sy = y2 - y1;

    if (sy < 0 || (sy == 0 && sx < 0)) {
        k = x1; x1 = x2; x2 = k;
        k = y1; y1 = y2; y2 = k;
        sx = -sx;
        sy = -sy;
    }

    if (sx > 0) dx1 = 1;
    else if (sx < 0) dx1 = -1;
    else dx1 = 0;

    if (sy > 0) dy1 = 1;
    else if (sy < 0) dy1 = -1;
    else dy1 = 0;

    m = abs(sx);
    n = abs(sy);
    dx2 = dx1;
    dy2 = 0;

    if (m < n) {
        m = abs(sy);
        n = abs(sx);
        dx2 = 0;
        dy2 = dy1;
    }

    x = x1; y = y1;
    cnt = m + 1;
    k = n / 2;

    while (cnt--) {
        if ((y >= 0) && (y < res[1])) {
            if (x < contour_x[y][0]) contour_x[y][0] = x;
            if (x > contour_x[y][1]) contour_x[y][1] = x;
        }

        k += n;
        if (k < m) {
            x += dx2;
            y += dy2;
        }
        else {
            k -= m;
            x += dx1;
            y += dy1;
        }
    }
}

void render_hori_line(SDL_Renderer* renderer, int x1, int x2, int y) {
    SDL_Rect r;
    r.x = x1; r.w = x2-x1;
    r.y = y;  r.h = 1;
    SDL_RenderFillRect(renderer, &r);
}

void transform_point(struct Camera* C, double P[], double point_2d[]) {

    double D[3];
    vector_subtract(P, C->pos, D);
    vector_transform(C->rotation_matrix_inv, D);

    point_2d[0] = C->res[0] / 2 + (D[0]*C->res[0]) / (D[2]*C->array_width) *
            C->focal_length;
    point_2d[1] = C->res[1] / 2 - (D[1]*C->res[1]) / (D[2]*C->array_height) *
            C->focal_length;

    /*
    double X = P[0] - C->pos[0];
    double Y = P[1] - C->pos[1];
    double Z = P[2] - C->pos[2];

    double dx = C->frame_cos[1] * X - C->frame_sin[1] * Z;
    double dy = C->frame_sin[0] *
               (C->frame_cos[1] * Z + C->frame_sin[1] * X) + C->frame_cos[0] * Y;
    double dz = C->frame_cos[0] *
               (C->frame_cos[1] * Z + C->frame_sin[1] * X) - C->frame_sin[0] * Y;

    point_2d[0] = (C->res[0] / 2 + (dx*C->res[0]) / (dz*C->array_width) *
            C->focal_length);
    point_2d[1] = (C->res[1] / 2 - (dy*C->res[1]) / (dz*C->array_height) *
            C->focal_length);
   */
}
