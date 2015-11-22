#include "projector.h"

static int objc; /* amount of objects */
static int objb = OBJECTS_START_BUFFER; /* buffer size for object array */
static struct Object* objs; /* array of object */

void PRJ_add_object(char* obj_file_path,
                    double pos[],
                    uint8_t color[],
                    double brightness,
                    double reflectiveness) {
    /* allocate memory for array */
    if (objc == 0) {
        objs = (struct Object*)malloc(sizeof(struct Object)*objb);
    }
    /* allocate more memory if the object array is about to become full */
    if (objc == objb-1) {
        objb += OBJECTS_START_BUFFER;
        objs = (struct Object*)realloc(objs, sizeof(struct Object)*objb);
    }
    object_create(obj_file_path, pos, color, brightness, reflectiveness,
                  &objs[objc]);
    objc++;
}


void PRJ_render(double pos[], double rot[], int res[],
                double fov, double focal_length) {

    struct Camera C = *camera_create(pos, rot, res, fov, focal_length);
    struct Bitmap bitmap = *bitmap_create(res);

    for (int row = 0; row < res[1]; row++) {
        for (int col = 0; col < res[0]; col++) {
            print_progress_bar((double)(row*res[0]+col)/(res[0]*res[1]));
            uint8_t color[3];
            calc_pixel_color(C, row, col, color);
            set_pixel(&bitmap, col, row, color);
        }
    }

    save_bitmap_to_png(&bitmap, "out.png");
}

static void calc_pixel_color(struct Camera C, int row, int col, uint8_t color[]) {
    double V[3];
    bool collide = false;
    double distance;
    printf("%f\n", C.focal_length);
    calc_camera_vector(C, row, col, V);
    for (int ob = 0; ob < objc; ob++) {
        for (int tr = 0; tr < objs[ob].tric; tr++) {
            double P[3];
            if (calc_ray_collision(C.pos, V, objs[ob].tris[tr], P)) {
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

    for (int i = 0; i < 3; i++) {
        color[i] = c;
    }
}

static void rotate_vertex_x(double vertex[], double point[], double ang) {
    
}

static void rotate_vertex_y(double vertex[], double point[], double ang) {

}


static void calc_camera_vector(struct Camera C, int row, int col, double V[]) {
    V[0] = -C.array_size[0]/2 + (col+0.5)*C.pixel_size;
    V[1] = -C.array_size[1]/2 + (row+0.5)*C.pixel_size;
    V[2] = C.focal_length;
    
    rotate_vertex_x(V, C.pos, C.rot[0]);
    rotate_vertex_y(V, C.pos, C.rot[1]); 
}

static bool calc_ray_collision(double S[], double V[], 
                               struct Triangle T, double P[]) {
    /* calculates coordinates where (point P) ray hits a triangle's plane
     * and returns true if P is inside the triangle */ 
    calc_collision_point_plane(S, V, T, P);
    bool point_in_triangle = calc_point_in_triangle(P, T);
    return point_in_triangle;
}

static void calc_collision_point_plane(double S[], double V[], 
                                       struct Triangle T, double P[]) {
    /* Points on ray can be expressed:
     * P(t) = S + tV
     * where S is the starting point and V is the direction vector.
     * to calc what t is where the ray collides with a triangle's plane:
     * t = -( N*S - N*T1 ) / ( N*V ) 
     * N is the plane's normal
     * T1 can be any vertex of the triangle */
    double numerator = T.D - dot_product(T.N, S);
    double denominator = dot_product(T.N, V);
    double t = numerator / denominator;

    /* P = S + tV */
    double tV[3];
    vector_multiply(t, V, tV);
    vector_add(S, tV, P);
}

static bool calc_point_in_triangle(double P[], struct Triangle T) {
    /* R = P - T1
     * Q1 = T2 - T1
     * Q2 = T3 - T1
     * M1 = */
    double R[3];
    vector_subtract(P, T.P1, R);
    double M2[][2] = {{dot_product(R, T.Q1)},
                      {dot_product(R, T.Q2)}};
    double W[][3] = {0};
    matrix_product(2, 2, M2, 2, 3, T.M, W);
    bool point_in_triangle = true;
    for (int i = 0; i < 3; i++) {
        if (W[0][i] < 0) point_in_triangle = false;
    }
    return point_in_triangle;
}

static void print_progress_bar(double progress) {
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
    int bar_current = progress*bar_total;
    int bar_remaining = bar_total-bar_current;

    for (int i = 0; i < bar_current; i++) current_str[i] = '#';
    for (int i = 0; i < bar_remaining; i++) remaining_str[i] = '-';
    current_str[bar_current] = 0;
    remaining_str[bar_remaining] = 0;
    printf("\rRendering... %d%% %ds [%s%s]",
           (int)(progress*100), eta, current_str, remaining_str);
}
