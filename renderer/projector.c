#include "projector.h"

struct Bitmap* projector_render(struct World W, struct Camera C) {
    struct Bitmap* bitmap = bitmap_create(C.res);

    for (int row = 0; row < C.res[1]; row++) {
        for (int col = 0; col < C.res[0]; col++) {
            print_progress_bar((double)(row*C.res[0]+col)/(C.res[0]*C.res[1]));
            uint8_t color[3];
            calc_pixel_color(W, C, row, col, color);
            set_pixel(bitmap, col, row, color);
        }
    }
    printf("\n");
    
    return bitmap;
}

void calc_pixel_color(struct World W,
                      struct Camera C,
                      int row, int col,
                      uint8_t color[]) {
    double V[3];
    bool collide = false;
    double distance;
    camera_calc_direction(C, row, col, V);
    for (int ob = 0; ob < W.object_count; ob++) {
        for (int tr = 0; tr < W.objects[ob].tric; tr++) {
            double P[3];
            if (calc_ray_collision(C.pos, V, W.objects[ob].tris[tr], P)) {
                double difference_vector[3];
                vector_subtract(P, C.pos, difference_vector);
                double d = magnitude(difference_vector);
                if (!collide) {
                    distance = d;
                    collide = true;
                }
                else if (d < distance) {
                    distance = d;
                }
            }
        }
    }

    int c = (collide) ? 255 : 0;
    for (int i = 0; i < 3; i++) color[i] = c;
}

bool calc_ray_collision(double S[], double V[], 
                        struct Triangle T, double P[]) {
    /* calculates coordinates where (point P) ray hits a triangle's plane
     * and returns true if P is inside the triangle */ 
    bool collides_with_plane = calc_collision_point_plane(S, V, T, P);
    bool point_in_triangle = calc_point_in_triangle(P, T);
    return collides_with_plane && point_in_triangle;
}

bool calc_collision_point_plane(double S[], double V[], 
                                struct Triangle T, double P[]) {
    /* Points on ray can be expressed:
     * P(t) = S + tV
     * where S is the starting point and V is the direction vector.
     * t is where the ray collides with a triangle's plane:
     * t = -(N*S-N*T1)/(N*V) = -(N*S+D)/(N*V) = (-D-N*S)/(N*V)
     * N is the plane's normal
     * D = -N*T1 becomes signed distance
     * T1 can be any vertex of the triangle */
    double numerator = -T.D - dot_product(T.N, S);
    double denominator = dot_product(T.N, V);
    double t = numerator / denominator;

    /* P = S + tV */
    double tV[3];
    vector_multiply(t, V, tV);
    vector_add(S, tV, P);

    /* return if point is in front of camera */
    return (t > 0);
}

bool calc_point_in_triangle(double P[], struct Triangle T) {
    /* R = P - T1
     * Q1 = T2 - T1
     * Q2 = T3 - T1 */
    double R[3];
    vector_subtract(P, T.P1, R);
    double M2[][1] = {{dot_product(R, T.Q1)},
                      {dot_product(R, T.Q2)}};
    double W[2][1];
    matrix_product(2, 2, T.M, 2, 1, M2, W);
    bool point_in_triangle = true;
    double w[] = {1-W[0][0]-W[1][0], W[0][0], W[1][0] };
    for (int i = 0; i < 3; i++) {
        if (w[i] < 0) point_in_triangle = false;
    }
    return point_in_triangle;
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
