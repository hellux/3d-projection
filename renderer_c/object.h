#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "vector.h"
#include "matrix.h"

#define MAX_VERTICES 200000
#define MAX_TRIANGLES 100000

#define OBJ_LINE_BUFFER_SIZE 128

struct Triangle {
    /* vertices / points */
    double P1[3];
    double P2[3];
    double P3[3];

    /* vectors & normal */
    double Q1[3];
    double Q2[3];
    double N[3];

    /* matrix */
    double M[2][2];
    
    double D;
};

struct Object {
    struct Triangle* tris;   /* array of pointers to triangle structs */
    int tric;                /* amount of triangles */
    uint8_t color[3];            /* rgb color */
    double brightness;       /* brigtness of object surface */
    double reflectiveness;   /* reflectiveness -||- */
};

static void triangle_create(double vertices[][3],
                            int indices[],
                            struct Triangle* T);
void object_create(char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   double brightness,
                   double reflectiveness,
                   struct Object* O);
static void load_obj_file(char* file_path,
                          int* vertexc, double vertices[][3],
                          int* indexc, int indices[][3]);
static void parse_line(char* line_buffer,
                       int* vertexc, double vertices[][3],
                       int* indexc, int indices[][3]);
