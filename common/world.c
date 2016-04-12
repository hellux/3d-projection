#include "world.h"

struct World* world_create() {
    struct World* W = malloc(sizeof(struct World));
    world_allocate_memory_objects(W);
    world_allocate_memory_triangles(W);
    return W;
}

void world_allocate_memory_objects(struct World* W) {
    W->objects_buffer = WORLD_OBJECTS_START_BUFFER;
    W->objects = calloc(sizeof(struct Object), W->objects_buffer);
    W->object_count = 0;
}

void world_reallocate_memory_objects(struct World* W) {
    W->objects_buffer += WORLD_OBJECTS_START_BUFFER;
    W->objects = realloc(W->objects, sizeof(struct Object)*W->objects_buffer);
}

void world_allocate_memory_triangles(struct World* W) {
    W->triangles_buffer = WORLD_TRIANGLES_START_BUFFER;
    W->triangles = calloc(sizeof(struct Triangle*), W->triangles_buffer);
    W->triangle_count = 0;
}

void world_reallocate_memory_triangles(struct World* W) {
    W->triangles_buffer += WORLD_TRIANGLES_START_BUFFER;
    W->triangles = realloc(W->triangles, 
                           sizeof(struct Triangle*)*W->triangles_buffer);
}

bool world_add_object(struct World* W,
                      const char* obj_file_path,
                      double pos[],
                      uint8_t color[],
                      double reflectiveness) {
    if (W->object_count == W->objects_buffer-1)
        world_reallocate_memory_objects(W);
    
    struct Object* object = &W->objects[W->object_count];
    bool success = object_create(obj_file_path,
                                 pos,
                                 color,
                                 reflectiveness,
                                 object);
    if (success) world_add_triangles(W, object->tris);
    W->object_count++;
    return success;
}

void world_add_triangles(struct World* W, struct Triangle* triangles) {
    for (int t = 0; t < W->objects[W->object_count].tric; t++) {
        if (W->triangle_count == W->triangles_buffer-1)
            world_reallocate_memory_triangles(W);
        W->triangles[W->triangle_count++] = &triangles[t];
    }
}
