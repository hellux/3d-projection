#ifndef config_h
#define config_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <libconfig.h>

#include "camera.h"
#include "object.h"
#include "world.h"

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera);
bool config_read_camera(config_t cfg, struct Camera* camera);
bool config_read_objects(config_t cfg, struct World* world);
bool config_read_object(config_setting_t* object, struct World* world);
bool config_write_camera(const char* input_cfg,
                         const char* output_cfg,
                         struct Camera* camera);

#endif
