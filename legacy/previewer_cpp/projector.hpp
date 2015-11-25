#include <SDL2/SDL.h>
#include <fstream>

#define MAX_OBJECTS 20
#define MAX_VERTICES 100000
#define CAMERA_SPEED 1
#define CAMERA_VELOCITY_LOSS 0.9
#define CAMERA_SLOW_MULTIPLIER 0.1
#define MOUSE_SENSITIVITY 0.0001

void PRJ_load_obj_file(std::string obj,
                       double pos[],
                       double ang[]);
void PRJ_update(int fps);
void PRJ_update_rotation(void);
void PRJ_update_position(int fps);
void PRJ_render(SDL_Renderer* renderer, int res[]);
void PRJ_render_object(SDL_Renderer* renderer,
                       int obj,
                       int res[]);
void PRJ_render_point(SDL_Renderer* renderer,
                      double vertex[],
                      int res[]);
void PRJ_transform_point(int point_2d[],
                         double vertex[],
                         double camera[],
                         int res[]);
void PRJ_move_forward(void);
void PRJ_move_back(void);
void PRJ_move_left(void);
void PRJ_move_right(void);
void PRJ_move_up(void);
void PRJ_move_down(void);
void PRJ_move_horizontal(double d, double v);
void PRJ_move_vertical(double d);
void PRJ_rotate(int u, int v);
void PRJ_fly_relative(int d);
void PRJ_accelerate(int i, double a);
void PRJ_zoom(double m);
void PRJ_set_dimensions(int w, int h);
void PRJ_set_slow(bool s);
