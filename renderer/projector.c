#include "projector.h"

struct Bitmap* projector_render(struct World W, struct Camera C) {
    struct Bitmap* bitmap = bitmap_create(C.res);
    
    printf("\n");

    printf("Objects to render: %d\n", W.object_count);
    printf("Resolution: %dx%d\n", C.res[0], C.res[1]);
    printf("Camera position: %f, %f, %f\n", C.pos[0], C.pos[1], C.pos[2]);
    printf("Camera rotation: %f, %f, %f\n", C.rot[0], C.rot[1], C.rot[2]);

    printf("\n");
    for (int row = 0; row < C.res[1]; row++) {
        for (int col = 0; col < C.res[0]; col++) {
            print_progress_bar((double)(row*C.res[0]+col)/(C.res[0]*C.res[1]));
            uint8_t pixel_color[3];
            calc_pixel_color(W, C, row, col, pixel_color);
            set_pixel(bitmap, col, row, pixel_color);
        }
    }
    printf("\n");
    
    return bitmap;
}

void calc_pixel_color(struct World W,
                      struct Camera C,
                      int row, int col,
                      uint8_t color[]) {
    double pixel_vector[3];
    camera_calc_pixel_direction(C, row, col, pixel_vector);
    
    uint8_t* surface_color;
    double depth = 0;
    /* calc closest surface */
    for (int ob = 0; ob < W.object_count; ob++) {
        for (int tr = 0; tr < W.objects[ob].tric; tr++) {
            double t = calc_ray_triangle_collision(C.pos,
                                                   pixel_vector,
                                                   W.objects[ob].tris[tr]);
            if (t > 0 && (t < depth || depth == 0)) {
                surface_color = W.objects[ob].color;
                depth = t;
            }
        }
    }
    if (depth == 0) {
        for (int i = 0; i < 3; i++) color[i] = 0;
    }
    else {
        for (int i = 0; i < 3; i++) color[i] = surface_color[i];
    }
}

double calc_ray_triangle_collision(double S[], double V[], struct Triangle T) {
    /* calculates coordinates where (point P) ray hits a triangle's plane
     * and returns true if P is inside the triangle */ 
    double t = calc_ray_plane_collision(S, V, T);
    if (t > 0) {
        double P[3], tV[3];
        vector_multiply(t, V, tV);
        vector_add(S, tV, P);
        if (calc_point_in_triangle(P, T)) return t;
        else return 0;
    }
    else {
        return 0;
    }
}

double calc_ray_plane_collision(double S[], double V[], struct Triangle T) {
    /* points on ray can be expressed:
     * P(t) = S + tV
     * where S is the starting point and V is the direction vector.
     * t is where the ray collides with a triangle's plane:
     * t = -(N*S-N*T1)/(N*V) = -(N*S+D)/(N*V) = (-D-N*S)/(N*V)
     * N is the plane's normal
     * D is plane's signed distance
     * T1 can be replaced by any vertex of the triangle */
    double numerator = -T.signed_distance - dot_product(T.normal, S);
    double denominator = dot_product(T.normal, V);
    if (denominator == 0) return 0; /* ray is parallell to plane */
    return numerator / denominator;
}

bool calc_point_in_triangle(double P[], struct Triangle T) {
    /* R = P - T1
     * Q1 = T2 - T1
     * Q2 = T3 - T1 */
    double R[3];
    vector_subtract(P, T.vertices[0], R);
    double M2[][1] = {{dot_product(R, T.Q1)},
                      {dot_product(R, T.Q2)}};
    double W[2][1];
    matrix_product(2, 2, T.barycentric_matrix, 2, 1, M2, W);
    double w[] = {1-W[0][0]-W[1][0], W[0][0], W[1][0]};
    for (int i = 0; i < 3; i++) {
        if (w[i] < 0) return false;
    }
    return true;
}

void print_progress_bar(double progress) {
    static time_t start;
    static char current_str[80];
    static char remaining_str[80];

    time_t eta;
    time_t elapsed;
    if (progress == 0) {
        start = time(0);
        return;
    }
    else {
        elapsed = time(0) - start;
        eta = (elapsed > 0) ? elapsed / progress - elapsed : 0;
    }
    
    int eta_length = (eta <= 0) ? 1 : log10(eta)+1;
    int bar_total = 80-21-eta_length;
    int bar_current = progress*bar_total+0.5;
    int bar_remaining = bar_total-bar_current;
    int percentage = progress*100+0.5;

    for (int i = 0; i < bar_current; i++) current_str[i] = '#';
    for (int i = 0; i < bar_remaining; i++) remaining_str[i] = '-';
    current_str[bar_current] = 0;
    remaining_str[bar_remaining] = 0;
    printf("\rRendering... %d%% %ds [%s%s]",
           percentage, (int)eta, current_str, remaining_str);
}
