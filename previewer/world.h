#ifndef world_h
#define world_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "object.h"

#define OBJECTS_START_BUFFER 3

struct World {
    struct Object* objects;
    size_t object_count;
    size_t objects_buffer_size;
};

struct World* world_create();
bool world_add_object(struct World* W,
                      const char* obj_file_path,
                      double pos[],
                      bool type);

#endif
