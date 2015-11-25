#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_VERTICES 200000
#define OBJ_LINE_BUFFER_SIZE 128

struct Object {
    double vertices[MAX_VERTICES][3];
    size_t vertexc;
    size_t vertexb;
    uint8_t color[3];
};

void object_create(char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O);
static void object_vertices_malloc(struct Object* O);
static void object_vertices_realloc(struct Object* O);
static void load_obj_file(struct Object* O, char* file_path);
static void parse_line(struct Object* O, char* line_buffer);

#endif
