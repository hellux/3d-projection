#include "projector.hpp"

double PRJ_dimensions[] = {8, 6, 10};     // dimension of camera
double PRJ_rotation[] = {0, 0};           // rotation of camera
double PRJ_position[] = {0, 1, 0};        // position of camera
double PRJ_velocity[] = {0, 0, 0};        // velocity of camera
double PRJ_acceleration[] = {0, 0, 0};    // acceleration of camera

double PRJ_objects[MAX_OBJECTS][MAX_VERTICES][3];   // list of object lists of vertices
int PRJ_objectCount = 0;                            // amount of objects
int PRJ_verticesCounts[MAX_OBJECTS];                // amount of vertices per object

bool PRJ_slow;                      // should camera move slow

double PRJ_COS[] = {1, 0};        // cosine of rotation for current frame
double PRJ_SIN[] = {0, 1};        // sine of rotation for current frame

void PRJ_loadObjFile(std::string obj_filename, double pos[], double ang[]) {
    std::ifstream input(obj_filename);
    std::string line;

    SDL_Log("Loading file \"%s\"...", obj_filename.c_str());
    int index = 0;
    while (std::getline(input, line)) {
        if (line.find("#") == -1 && line.find("v ") != -1) {
            int separator[4];
            int start = 0;
            for (int i = 0; i < 3; i++) {
                separator[i] = line.find(" ", start);
                start = line.find_first_not_of(" ", separator[i]);
            }
            separator[3] = line.length() - 1;
            double coords[3];
            for (int i = 0; i < 3; i++) {
                coords[i] = stod(line.substr(separator[i],
                            separator[i+1] - separator[i])) + pos[i];
            }
            PRJ_objects[PRJ_objectCount][index][0] = coords[0];
            PRJ_objects[PRJ_objectCount][index][1] = coords[1];
            PRJ_objects[PRJ_objectCount][index][2] = coords[2];
            index++;
        }
    }

    PRJ_verticesCounts[PRJ_objectCount] = index;
    PRJ_objectCount++;
    SDL_Log("Vertex count for \"%s\": %d", obj_filename.c_str(), index);
}

void PRJ_saveConfigFile() {
}

void PRJ_update(int fps) {
    PRJ_updateRotation();
    PRJ_updatePosition(fps);
}

void PRJ_updateRotation() {
    for (int i = 0; i < 2; i++) {
        PRJ_COS[i] = cos(PRJ_rotation[i]);
        PRJ_SIN[i] = sin(PRJ_rotation[i]);
    }
}

void PRJ_updatePosition(int fps) {
    fps++;
    for (int i = 0; i < 3; i++) {
        PRJ_velocity[i] += PRJ_acceleration[i]/(fps);
        PRJ_velocity[i] *= CAMERA_VELOCITY_LOSS;
        PRJ_position[i] += PRJ_velocity[i]/(fps);
    }
}

void PRJ_render(SDL_Renderer* renderer, int res[]) {
    for (int obj = 0; obj < PRJ_objectCount; obj++) {
        PRJ_renderObject(renderer, obj, res);
    }
}

void PRJ_renderObject(SDL_Renderer* renderer,
                      int obj,
                      int res[]) {
    for (int p = 0; p < PRJ_verticesCounts[obj]; p++) {
        PRJ_renderPoint(renderer, PRJ_objects[obj][p], res);
    }
}

void PRJ_renderPoint(SDL_Renderer* renderer,
                     double vertex[],
                     int res[]) {
    int point_2d[2];
    PRJ_transformPoint(point_2d,
                       vertex,
                       PRJ_position,
                       res);
    SDL_RenderDrawPoint(renderer, point_2d[0], point_2d[1]);
}

void PRJ_transformPoint(int point_2d[],
                        double vertex[],
                        double camera[],
                        int res[]) {
    double* R = PRJ_dimensions;
    double X = vertex[0] - camera[0];
    double Y = vertex[1] - camera[1];
    double Z = vertex[2] - camera[2];

    double dx = PRJ_COS[1] * X - PRJ_SIN[1] * Z;
    double dy = PRJ_SIN[0] * (PRJ_COS[1] * Z + PRJ_SIN[1] * X) + PRJ_COS[0] * Y;
    double dz = PRJ_COS[0] * (PRJ_COS[1] * Z + PRJ_SIN[1] * X) - PRJ_SIN[0] * Y;

    point_2d[0] = (int) (res[0] / 2 + (dx * res[0]) / (dz * R[0]) * R[2]);
    point_2d[1] = (int) (res[1] / 2 - (dy * res[1]) / (dz * R[1]) * R[2]);
}

void PRJ_setSlow( bool s ) { PRJ_slow = s; }
void PRJ_moveForward() { PRJ_moveHorizontal(-CAMERA_SPEED, PRJ_rotation[1]); }
void PRJ_moveBack() { PRJ_moveHorizontal(CAMERA_SPEED, PRJ_rotation[1]); }
void PRJ_moveRight() { PRJ_moveHorizontal(-CAMERA_SPEED, PRJ_rotation[1] + M_PI / 2); }
void PRJ_moveLeft() { PRJ_moveHorizontal(CAMERA_SPEED, PRJ_rotation[1] + M_PI / 2); }
void PRJ_moveUp() { PRJ_moveVertical(CAMERA_SPEED); }
void PRJ_moveDown() { PRJ_moveVertical( -CAMERA_SPEED ); }

void PRJ_moveHorizontal( double d, double v ) {
    if ( PRJ_slow ) d *= CAMERA_SLOW_MULTIPLIER;
    PRJ_accelerate( 0, -d * sin(v) );
    PRJ_accelerate( 2, -d * cos(v) );
}
void PRJ_moveVertical( double d ) {
    if ( PRJ_slow ) d *= CAMERA_SLOW_MULTIPLIER;
    PRJ_accelerate( 1, d );
}

void PRJ_accelerate(int i, double a) { PRJ_velocity[i] += a; }

void PRJ_rotate(int u, int v) {
    PRJ_rotation[0] = fmod( PRJ_rotation[0] + v * MOUSE_SENSITIVITY, M_PI * 2 );
    PRJ_rotation[1] = fmod( PRJ_rotation[1] + u * MOUSE_SENSITIVITY, M_PI * 2 );
}

void PRJ_setDimensions( int w, int h ) {
    PRJ_dimensions[2] = w / ( PRJ_dimensions[0] * PRJ_dimensions[2] );
    PRJ_dimensions[0] = w / 100.0;
    PRJ_dimensions[1] = h / 100.0;
}
