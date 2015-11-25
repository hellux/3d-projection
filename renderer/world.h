#ifndef world_h
#define world_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "object.h"

#define OBJECTS_START_BUFFER 3

struct World {
    size_t object_count;
    size_t objects_buffer;
    struct Object* objects;
};

struct World* world_create();
void world_add_object(struct World* W,
                      char* obj_file_path,
                      double pos[],
                      uint8_t color[],
                      double brightness,
                      double reflectiveness);

#endif
