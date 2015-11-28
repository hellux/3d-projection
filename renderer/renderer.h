#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <libconfig.h>

#include "projector.h"
#include "world.h"
#include "camera.h"

#include "matrix.h"
#include "vector.h"

bool config_parse(const char* cfg_path, struct World* world, struct Camera* camera);
bool config_add_camera(config_t cfg, struct Camera* camera);
bool config_add_objects(config_t cfg, struct World* world);
bool config_add_object(config_setting_t* object, struct World* world);
