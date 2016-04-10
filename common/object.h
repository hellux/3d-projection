#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "triangle.h"

#define OBJ_LINE_BUFFER_SIZE 256
#define OBJECT_TRIANGLES_START_BUFFER 10000
#define OBJECT_VERTICES_START_BUFFER 10000

struct Object {
    struct Triangle* tris;
    size_t tris_buffer;
    size_t tric;

    double** verts;
    double** verts_2d;
    bool* verts_in_front;
    size_t verts_buffer;
    size_t vertc;

    uint8_t color[3];
    double color_hsl[3];
};

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O);
void object_allocate_memory_triangles(struct Object* O);
void object_reallocate_memory_triangles(struct Object* O);
void object_allocate_memory_vertices(struct Object* O);
void object_reallocate_memory_vertices(struct Object* O);
void object_allocate_memory_verts_in_front(struct Object* O);
void object_allocate_memory_verts_2d(struct Object* O);
void object_set_color(struct Object* O, uint8_t color[]);
void object_adjust_position(struct Object* O, double pos[]);
bool object_parse(struct Object* O, const char* obj_file_path);
void object_parse_line(char* line_buffer, struct Object* O);
void object_add_vertex(struct Object* O, double vertex[]);
void object_create_triangle(struct Object* O, int indices[]);

#endif
