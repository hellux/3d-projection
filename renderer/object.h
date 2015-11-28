#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "triangle.h"

#define MAX_VERTICES 200000
#define MAX_TRIANGLES 100000

#define OBJ_LINE_BUFFER_SIZE 128

struct Object {
    struct Triangle* tris;   /* array of pointers to triangle structs */
    int tric;                /* amount of triangles */
    uint8_t color[3];        /* rgb color */
    bool light;              /* is light source */
};

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   bool light,
                   struct Object* O);
bool load_obj_file(const char* file_path,
                   int* vertexc, double vertices[][3],
                   int* indexc, int indices[][3]);
void parse_line(char* line_buffer,
                int* vertexc, double vertices[][3],
                int* indexc, int indices[][3]);

#endif
