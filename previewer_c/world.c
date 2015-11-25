#include "world.h"

struct World* world_create() {
    struct World* W = malloc(sizeof(struct World));

    W->objects_buffer_size = OBJECTS_START_BUFFER;
    W->objects = (struct Object*)calloc(sizeof(struct Object),
                                        W->objects_buffer_size);
    W->object_count = 0;

    return W;
}

void world_add_object(struct World* W,
                      char* obj_file_path,
                      double pos[],
                      int type) {
    /* allocate more memory if the object array is about to become full */
    if (W->object_count == W->objects_buffer_size - 1) {
        W->objects_buffer_size += OBJECTS_START_BUFFER;
        W->objects = (struct Object*)realloc(W->objects, 
                                  sizeof(struct Object)*W->objects_buffer_size);
    }
    uint8_t color[] = {0xFF, 0xFF, 0xFF};
    switch (type) {
        case 0:
            break;
        case 1:
            color[2] = 0x00;
            break;
    }

    object_create(obj_file_path, pos, color, &W->objects[W->object_count]);
    W->object_count++;
}

