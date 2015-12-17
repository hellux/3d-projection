#include "world.h"

struct World* world_create() {
    struct World* W = malloc(sizeof(struct World));

    W->objects_buffer = OBJECTS_START_BUFFER;
    W->objects = (struct Object*)calloc(sizeof(struct Object),
                                        W->objects_buffer);
    W->object_count = 0;

    W->triangles_buffer = TRIANGLES_START_BUFFER;
    W->triangles = (struct Triangle**)calloc(sizeof(struct Triangle*),
                                             W->triangles_buffer);
    W->triangle_count = 0;

    return W;
}

bool world_add_object(struct World* W,
                      const char* obj_file_path,
                      double pos[],
                      uint8_t color[],
                      bool light) {
    /* reallocate more memory for objects array if it is about to become full */
    if (W->object_count == W->objects_buffer-1) {
        W->objects_buffer += OBJECTS_START_BUFFER;
        W->objects = (struct Object*)realloc(W->objects,
                                     sizeof(struct Object)*W->objects_buffer);
    }
   
    /* return true if object was successfully created */ 
    bool success = object_create(obj_file_path,
                                 pos,
                                 color,
                                 light,
                                 &(W->objects[W->object_count++]));

    if (success) {
        for (int o = 0; o < W->object_count; o++) {
            for (int t = 0; t < W->objects[o].tric; t++) {
                if (W->triangle_count == W->triangles_buffer-1) {
                    W->triangles_buffer += TRIANGLES_START_BUFFER;
                    W->triangles = (struct Triangle**)realloc(W->triangles,
                                      sizeof(struct Triangle*)*W->triangles_buffer);
                }
                W->triangles[W->triangle_count++] = &W->objects[o].tris[t];
            }
        }
    }
    return success;
}
