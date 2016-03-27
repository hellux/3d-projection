#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "projector.h"
#include "../common/world.h"
#include "../common/camera.h"
#include "../common/config.h"
#include "../common/matrix.h"
#include "../common/vector.h"

#define CONTINUE 2

int handle_args(int argc, char* args[], char** config_path, char** output_path);
