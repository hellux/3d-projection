#include "projector.h"

/* camera properties */
double rotation[] = {0, 0};
double position[] = {0, 0, 0};

int objc; /* amount of objects */
int objb = OBJECTS_START_BUFFER;
struct Object* objs;

void PRJ_add_object(char* obj_file_path,
                    double pos[],
                    double rot[],
                    int color[],
                    double brightness,
                    double reflectiveness) {
    /* allocate memory for array */
    if (objc == 0) {
        objs = (struct Object*)malloc(sizeof(struct Object)*objb);
    }
    /* allocate more memory if the object array is about to become full */
    else if (objc == objb - 1) {
        objb += OBJECTS_START_BUFFER;
        objs = (struct Object*)realloc(objs, sizeof(struct Object)*objb);
    }
    
    object_create(obj_file_path, pos, rot, color, brightness, reflectiveness,
                  &objs[objc]);

    objc++;
}

bool _calc_ray_collision(double S[], double V[], 
                         struct Triangle T, double P[]) {
    /* calculates coordinates where (point P) ray hits a triangle's plane
     * and returns true if P is inside the triangle */ 
    _calc_collision_point_plane(S, V, T, P);
    bool point_in_triangle = _calc_point_in_triangle(P, T);
    return point_in_triangle;
}

void _calc_collision_point_plane(double S[], double V[], 
                                 struct Triangle T, double P[]) {
    /* Points on ray can be expressed:
     * P(t) = S + tV
     * where S is the starting point and V is the direction vector.
     * to calc what t is where the ray collides with a triangle's plane:
     * t = -( N*S - N*T1 ) / ( N*V ) 
     * N is the plane's normal
     * T1 can be any vertex of the triangle */
    double denominator = dot_product(T.N, V);
    double numerator = -dot_product(T.N, S) - T.D;
    double t = numerator / denominator;
    
    /* P = S + tV */
    double tV[3];
    vector_multiply(t, V, tV);
    vector_add(S, tV, P);
}

bool _calc_point_in_triangle(double P[], struct Triangle T) {
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
