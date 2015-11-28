#include "renderer.h"

int main(int argc, char* args[]) {
    struct World world;
    struct Camera camera;
    struct Bitmap bitmap;

    if (!config_parse("render.cfg", &world, &camera)) {
        return EXIT_FAILURE;
    }
    
    bitmap = *projector_render(world, camera);
    if (save_bitmap_to_png(&bitmap, "out.png")) {
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }

}

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera) {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, cfg_path)) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        fprintf(stderr, "Could not parse config file '%s'.\n", cfg_path);
        return false;
    }
    
    /* return if the camera and at least one object was loaded successfully */
    bool valid_config = (config_add_camera(cfg, camera) && 
                         config_add_objects(cfg, world));
    config_destroy(&cfg);
    return valid_config;
}

bool config_add_camera(config_t cfg, struct Camera* camera) {
    config_setting_t *stn_cam = config_lookup(&cfg, "camera");
    if (stn_cam == NULL) {
        fprintf(stderr, "Camera settings not found.");
        return false;
    }

    double pos[] = {0, 0, 0}, rot[] = {0, 0, 0};
    int res[2];
    double fov, focal_length;

    /* parse position, rotation and resolution */
    config_setting_t* stn_pos = config_setting_lookup(stn_cam, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) { 
            pos[i] = config_setting_get_float_elem(stn_pos, i);
        }
    }
    config_setting_t* stn_rot = config_setting_lookup(stn_cam, "rot");
    if (stn_rot != NULL) {
        for (int i = 0; i < 3; i++) { 
            rot[i] = config_setting_get_float_elem(stn_rot, i);
        }
    }
    config_setting_t* stn_res = config_setting_lookup(stn_cam, "res");
    if (stn_res != NULL) {
        for (int i = 0; i < 2; i++) { 
            res[i] = config_setting_get_int_elem(stn_res, i);
        }
    }

    /* parse field of view and focal length */
    config_setting_lookup_float(stn_cam, "fov", &fov);
    config_setting_lookup_float(stn_cam, "focal_length", &focal_length);
    
    /* create camera */
    struct Camera* c = camera_create(pos, rot, res, fov, focal_length);
    if (c == NULL) {
        return false;
    }
    else {
        *camera = *c;
        return true;
    }
}

bool config_add_objects(config_t cfg, struct World* world) {
    config_setting_t *object_list;
    bool has_objects = false;

    /* create world struct */
    *world = *world_create();

    object_list = config_lookup(&cfg, "objects");
    if (object_list != NULL) {
        int count = config_setting_length(object_list);
        for (int i = 0; i < count; i++) {
            config_setting_t* object = config_setting_get_elem(object_list, i);
            if (object != NULL) {
                if (config_add_object(object, world)) has_objects = true;
            }
        }
    }
    return has_objects; /* at least one object loaded successfully */
}

bool config_add_object(config_setting_t* object, struct World* world) {
    const char* obj_file_path;
    double pos[3];
    uint8_t col[3];
    int light;

    if (!config_setting_lookup_string(object, "path", &obj_file_path)) {
        fprintf(stderr, "No path to model in object section!\n");
        return false;
    }

    config_setting_t* stn_pos = config_setting_lookup(object, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) {
            pos[i] = config_setting_get_float_elem(stn_pos, i);
        }
    }
    config_setting_t* stn_col = config_setting_lookup(object, "col");
    if (stn_col != NULL) {
        for (int i = 0; i < 3; i++) {
            col[i] = config_setting_get_int_elem(stn_col, i);
        }
    }
    config_setting_lookup_bool(object, "light", &light);

    /* return if object was successfully added to world struct */
    return world_add_object(world, obj_file_path, pos, col, (bool)light);
}


