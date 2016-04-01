#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "triangle.h"

#define MAX_VERTICES 500000
#define MAX_TRIANGLES 500000

#define OBJ_LINE_BUFFER_SIZE 256

struct Object {
    struct Triangle* tris;
    int tric;
    double verts[MAX_VERTICES][3];
    bool verts_in_front[MAX_VERTICES];
    double verts_2d[MAX_VERTICES][2];
    int vertc;
    uint8_t color[3];
};

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O);
void object_set_color(struct Object* O, uint8_t color[]);
void object_adjust_position(struct Object* O, double pos[]);
bool object_parse(struct Object* O, const char* obj_file_path, int indices[][3]);
void object_parse_line(char* line_buffer,
                       int* vertexc, double vertices[][3],
                       int* indexc, int indices[][3]);
void object_create_triangles(struct Object* O, int indices[][3]);

#endif
