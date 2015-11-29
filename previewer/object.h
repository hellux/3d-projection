#ifndef object_h
#define object_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_VERTICES 200000
#define OBJ_LINE_BUFFER_SIZE 128

struct Object {
    double vertices[MAX_VERTICES][3];
    size_t vertexc;
    size_t vertexb;
    uint8_t color[3];
};

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O);
bool load_obj_file(struct Object* O, const char* file_path);
void parse_line(struct Object* O, char* line_buffer);

#endif
