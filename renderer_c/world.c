#include "world.h"

struct World* world_create() {
    struct World* W = malloc(sizeof(struct World));

    W->objects_buffer = OBJECTS_START_BUFFER;
    W->objects = (struct Object*)calloc(sizeof(struct Object),
                                        W->objects_buffer);
    W->object_count = 0;

    return W;
}

void world_add_object(struct World* W,
                      char* obj_file_path,
                      double pos[],
                      uint8_t color[],
                      double brightness,
                      double reflectiveness) {
    /* reallocate more memory for objects array if it is about to become full */
    if (W->object_count == W->objects_buffer -1) {
        W->objects_buffer += OBJECTS_START_BUFFER;
        W->objects = (struct Object*)realloc(W->objects,
                                     sizeof(struct Object)*W->objects_buffer);
    }
    
    object_create(obj_file_path,
                  pos,
                  color,
                  brightness,
                  reflectiveness,
                  &W->objects[W->object_count]);
    W->object_count++;
}
