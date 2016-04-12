#ifndef world_h
#define world_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "object.h"

#define WORLD_OBJECTS_START_BUFFER 3
#define WORLD_TRIANGLES_START_BUFFER 20000

struct World {
    size_t object_count;
    size_t objects_buffer;
    struct Object* objects;

    size_t triangle_count;
    size_t triangles_buffer;
    struct Triangle** triangles;
};

struct World* world_create();
void world_allocate_memory_objects(struct World* W);
void world_reallocate_memory_objects(struct World* W);
void world_allocate_memory_triangles(struct World* W);
bool world_add_object(struct World* W,
                      const char* obj_file_path,
                      double pos[],
                      uint8_t color[],
                      double reflectiveness);
void world_add_triangles(struct World* W, struct Triangle* triangles);

#endif
