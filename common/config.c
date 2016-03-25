#include "config.h"

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera) {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, cfg_path)) {
        fprintf(stderr, "previewer: %s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        fprintf(stderr, "previewer: could not parse config file '%s'\n", cfg_path);
        return false;
    }

    bool valid_config = (config_read_camera(cfg, camera) &&
                         config_read_objects(cfg, world));
    if (valid_config) {
        for (int o = 0; o < world->object_count; o++) {
            for (int t = 0; t < world->objects[o].tric; t++) {
                triangle_set_color(&world->objects[o].tris[t], camera->light);
            }   
        } 
    }
    config_destroy(&cfg);

    printf("Objects: %d\n", world->object_count);
    printf("Triangles: %d\n", world->triangle_count);
    printf("Resolution: %dx%d\n", camera->res[0], camera->res[1]);

    /* return if the camera and at least one object was loaded successfully */
    return valid_config;
}

bool config_read_camera(config_t cfg, struct Camera* camera) {
    config_setting_t *stn_cam = config_lookup(&cfg, "camera");
    if (stn_cam == NULL) {
        fprintf(stderr, "previewer: camera settings not found\n");
        return false;
    }

    double pos[] = {0, 0, 0}, rot[] = {0, 0, 0}, light[] = {0, 0, 0};
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
    config_setting_t* stn_lgt = config_setting_lookup(stn_cam, "light");
    if (stn_lgt != NULL) {
        for (int i = 0; i < 3; i++) {
            light[i] = config_setting_get_float_elem(stn_lgt, i);
        }
    }

    /* parse field of view and focal length */
    config_setting_lookup_float(stn_cam, "fov", &fov);
    config_setting_lookup_float(stn_cam, "focal_length", &focal_length);

    /* create camera */
    struct Camera* c = camera_create(pos, rot, res, fov, focal_length, light);
    if (c == NULL) {
        return false;
    }
    else {
        *camera = *c;
        return true;
    }
}

bool config_read_objects(config_t cfg, struct World* world) {
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
                if (config_read_object(object, world)) has_objects = true;
            }
        }
    }
    return has_objects; /* at least one object loaded successfully */
}

bool config_read_object(config_setting_t* object, struct World* world) {
    const char* obj_file_path;
    double pos[3];
    uint8_t col[3];

    if (!config_setting_lookup_string(object, "path", &obj_file_path)) {
        fprintf(stderr, "previewer: no path to model in object section\n");
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

    /* return if object was successfully added to world struct */
    return world_add_object(world, obj_file_path, pos, col);
}

bool config_write_camera(const char* in_path,
                         const char* out_path,
                         struct Camera* camera) {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, in_path)) {
        fprintf(stderr, "previewer: %s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        fprintf(stderr, "previewer: could not parse config file '%s'\n", in_path);
        return false;
    }

    config_setting_t* stn_cam = config_lookup(&cfg, "camera");
    if (stn_cam == NULL) {
        fprintf(stderr, "renderer: camera settings not found\n");
        return false;
    }

    config_setting_t* stn_pos = config_setting_lookup(stn_cam, "pos");
    if (stn_pos != NULL) {
        for (int i = 0; i < 3; i++) {
            config_setting_set_float_elem(stn_pos, i, camera->pos[i]);
        }
    }
    config_setting_t* stn_rot = config_setting_lookup(stn_cam, "rot");
    if (stn_rot != NULL) {
        for (int i = 0; i < 3; i++) {
            config_setting_set_float_elem(stn_rot, i, camera->rot[i]);
        }
    }
    config_setting_t* stn_lgt = config_setting_lookup(stn_cam, "light");
    if (stn_lgt != NULL) {
        for (int i = 0; i < 3; i++) {
            config_setting_set_float_elem(stn_lgt, i, camera->light[i]);
        }
    }

    config_setting_set_float(config_setting_lookup(stn_cam, "fov"),
                             camera->fov);
    config_setting_set_float(config_setting_lookup(stn_cam, "focal_length"),
                             camera->focal_length);

    if (!config_write_file(&cfg, out_path)) {
        fprintf(stderr, "previewer: could not write config file to '%s'", out_path);
        return false;
    }

    return true;
}
