#include "projector.h"

void render_scene(SDL_Renderer* renderer, struct World* W, struct Camera* C) {
    srand(time(NULL));
    for (int o = 0; o < W->object_count; o++) {
        /* flag all vertices as unrendered */
        object_clear_2d_array(&W->objects[o]);
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

void render_triangle(SDL_Renderer* renderer,
                     struct Camera* C,
                     struct Triangle* T) {
    for (int i = 0; i < 3; i++) {
		int index = T->indices[i];
		if (!T->object->verts_rendered[index]) {
			T->object->verts_rendered[index] = true;
            transform_point(C,
                            T->object->verts[index],
                            T->object->verts_2d[index]);
		}
	}
    SDL_SetRenderDrawColor(renderer, rand()%0xFF, rand()%0xFF, rand()%0xFF, 0xFF);
	render_fill_triangle(renderer,
					     T->object->verts_2d[T->indices[0]],
						 T->object->verts_2d[T->indices[1]],
						 T->object->verts_2d[T->indices[2]]);

}

int triangle_compare_depth(const void *t1, const void* t2) {
    /* return 
     * z1 < z2 : 1
     * z1 > z2 : -1
     * z1 = z2 : 0 */
    const struct Triangle** dt1 = (const struct Triangle**) t1;
    const struct Triangle** dt2 = (const struct Triangle**) t2;
    return ((*dt1)->depth < (*dt2)->depth) - ((*dt1)->depth > (*dt2)->depth); 
}

int vertex_compare_y(const void* v1, const void* v2) {
    /* return: 
     * y1 < y2 : 1
     * y1 > y2 : -1
     * y1 = y2 : 0 */
    const double** dv1 = (const double **) v1;
    const double** dv2 = (const double **) v2;
    return (*(*dv1+1) < *(*dv2+1)) - (*(*dv1+1) > *(*dv2+1)); 
}

void render_fill_triangle(SDL_Renderer* renderer, double v1[], 
                                                  double v2[],
                                                  double v3[]) {
    /* divides triangle to a top flat and a bottom flat triangle 
     * and renders them separately
     *                   x
     *                  |-|
     *        - - - - - __o v1  - -
     *      |     ___--- /|     | | v1.y - v2.y
     *   v2 o_-_--- - - o vn    | -
     *      |\         /  |     |
     *        \       /         | v1.y - v3.y
     *      |  \     /    |     |
     *          \   /           |
     *      |    \ /      |     |
     *        - - o - - -       -
     *           v3 
     *            |-------|
     *            v1.x-v3.y
     *
     * vn.x = v1 - x
     * vn.y = v2.y
     *
     * x / (v1.y-v2.y) = (v1.x-v3.x) / (v1.y-v3.y)
     * x = (v1.y-v2.y) * (v1.x-v3.x) / (v1.y-v3.x)
     * vn.x = v1 - (v1.y-v2.y) * (v1.x-v3.x) / (v1.y-v3.y)
     * */

    double* v[] = {v1, v2, v3};
	/* sort array by y ascending */
    qsort(v, 3, sizeof(double*), vertex_compare_y);

    if (v[1][1] == v[2][1]) {
        render_fill_bottom_flat_triangle(renderer,
                                         v[0],
                                         v[1],
                                         v[2]);
    }
    else if (v[0][1] == v[1][1]) {
        render_fill_top_flat_triangle(renderer,
                                      v[0],
                                      v[1],
                                      v[2]);
    }
    else {
        double vn[] = {v[0][0] - (v[0][1]-v[1][1]) * 
                        (double)(v[0][0]-v[2][0]) / (v[0][1]-v[2][1]),
                    v[1][1] };
        render_fill_top_flat_triangle(renderer,
                                      v[1],
                                      vn,
                                      v[2]);
        render_fill_bottom_flat_triangle(renderer,
                                         v[0],
                                         v[1],
                                         vn);
    }
}

void render_fill_top_flat_triangle(SDL_Renderer* renderer,
                                   double v1[], double v2[], double v3[]) {
    /*   v1     v2
     *    o-----o
     *     \   /
     *   k1 \ /  k2
     *       o v3 
     *
     * required: v1.y == v2.y > v3.y 
     *
     * k1 = deltay/deltax = (v1.y-v3.y) / (v1.x-v3.x)
     * k2 = deltax/deltay = (v2.y-v3.y) / (v2.x-v3.x)
     *
     * */

	double k1 = (v1[0] - v3[0]) / (v1[1] - v3[1]);
	double k2 = (v2[0] - v3[0]) / (v2[1] - v3[1]);
    
	double x1 = v3[0];
	double x2 = v3[0];

	for (int y = v3[1]; y <= v1[1]; y++) {
        render_hori_line(renderer, x1, x2, y);
		x1 += k1;
		x2 += k2;
	}
}

void render_fill_bottom_flat_triangle(SDL_Renderer* renderer,
                                      double v1[], double v2[], double v3[]) {
    /* 
     *    v1 o
     * k1x1 / \ k2x2
     *     /   \
     *    o-----o
     *   v2     v3 
     *  
     * required: v1.y > v2.y == v3.y */

	double k1 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
	double k2 = (v3[0] - v1[0]) / (v3[1] - v1[1]);

	double x1 = v1[0];
	double x2 = v1[0];

	for (int y = v1[1]; y >= v3[1]; y--) {
        render_hori_line(renderer, x1, x2, y);
		x1 -= k1;
		x2 -= k2;
	}
}

void render_hori_line(SDL_Renderer* renderer, int x1, int x2, int y) {
    SDL_Rect r;
    r.x = x1;    r.y = y;
    r.w = x2-x1; r.h = 1;
    SDL_RenderFillRect(renderer, &r);
}

void transform_point(struct Camera* C, double P[], double point_2d[]) {
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
}
