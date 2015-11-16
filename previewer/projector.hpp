#include <SDL2/SDL.h>
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>

#define MAX_OBJECTS 20
#define MAX_VERTICES 100000
#define CAMERA_SPEED 0.4
#define MOUSE_SENSITIVITY 0.0001

void PRJ_loadObjFile( std::string obj, 
                      double pos[],
                      double ang[] );

void PRJ_update( int fps );
void PRJ_updateRotation( void );
void PRJ_updatePosition( int fps );
void PRJ_render( SDL_Renderer* renderer, int res[] );
void PRJ_renderObject(SDL_Renderer* renderer,
	                  int obj,
                      int res[] );
void PRJ_renderPoint( SDL_Renderer* renderer,
	                  double vertex[],
                      int res[] );
void PRJ_transformPoint( int point_2d[],
                         double vertex[],
	                     double camera[],
                         int res[] );
void PRJ_moveForward( void );
void PRJ_moveBack( void );
void PRJ_moveLeft( void );
void PRJ_moveRight( void );
void PRJ_moveRelative( double d, double v );
void PRJ_rotate( int u, int v );
void PRJ_flyRelative( int d );
void PRJ_accelerate( int i, double a );
void PRJ_zoom( double m );
void PRJ_setDimensions( int w, int h );
void PRJ_setSlow( bool s );
