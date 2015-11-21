#include "previewer.hpp"

int resolution[] = { 1000, 700 };

SDL_DisplayMode displayMode;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int main(int argc, char* args[]) {

    initWindow();
    initProjector();

    bool quit = false;
    Uint64 tick = 0;
    int fps = 1000;
    SDL_Event event;

    SDL_PollEvent(&event);
    SDL_StopTextInput();

    // M A I N    L O O P
    while ( !quit ) {
        pollEvents( &event, &quit );
        handleControlKeys();
        handleKeys( &quit );
        render( &fps );

        //handle fps

        fps = SDL_GetPerformanceFrequency() /
            (SDL_GetPerformanceCounter() - tick);
        tick = SDL_GetPerformanceCounter();
    }

    cleanup();

    return 0;
}

void initWindow() {
    SDL_Log( "Initializing SDL %s. %s\n",
             ( SDL_Init( SDL_INIT_VIDEO ) == 0 ? "succeeded" : "failed" ),
             SDL_GetError() );
    SDL_SetRelativeMouseMode( SDL_TRUE );
    SDL_GetCurrentDisplayMode( 0, &displayMode );
    SDL_Log( "Current resolution: %dx%d", resolution[0], resolution[1] );
    SDL_Log( "Current refresh rate: %d", displayMode.refresh_rate );
    SDL_Log( "Current display format: %s", SDL_GetPixelFormatName( displayMode.format) );

    //window
    window = SDL_CreateWindow( "Preview",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                resolution[0],
                                resolution[1],
                                SDL_WINDOW_SHOWN );
    SDL_Log( "Creation of window %s. %s\n",
             ( window != NULL ? "succeeded" : "failed" ),
             SDL_GetError() );
}

void initProjector() {
    //renderer
    renderer = SDL_CreateRenderer( window, 1, SDL_RENDERER_ACCELERATED );
    if ( renderer == NULL ) {
        SDL_Log( "Renderer failed to initialize! %s", SDL_GetError() );
    }
    else {
        SDL_Log( "Renderer successfully initialized." );
    }

    PRJ_setDimensions( resolution[0], resolution[1] );

    //load files
    PRJ_loadObjFile( "../data/grid.obj",
                     new double[3] {0, 0, 0},
                     new double[2] {0, 0} );
    PRJ_loadObjFile( "../data/porsche-911.obj",
                     new double[3] {0, 0, 0},
                     new double[2] {0, 0} );
}

void pollEvents( SDL_Event* event, bool* quit ) {
    while ( SDL_PollEvent( event ) != 0 ) {
        switch ( event->type ) {
        //SYSTEM
        case SDL_QUIT:
            *quit = true;
            break;
        case SDL_WINDOWEVENT:
            handleWindowEvent( event );
            break;
        //CONTROLS
        case SDL_MOUSEMOTION:
            PRJ_rotate( event->motion.xrel, event->motion.yrel );
            break;
        case SDL_KEYDOWN:
            handleKeyDownEvent( event );
            break;
        case SDL_KEYUP:
            handleKeyUpEvent( event );
            break;
        }
    }
}

void handleWindowEvent( SDL_Event* event ) {
    switch ( event->window.event ) {
    case SDL_WINDOWEVENT_RESIZED:
        resolution[0] = event->window.data1;
        resolution[1] = event->window.data2;
        PRJ_setDimensions( resolution[0], resolution[1] );
        break;
    }
}

void handleKeyDownEvent( SDL_Event* event ) {
    switch ( event->key.keysym.scancode ) {
    case SDL_SCANCODE_LSHIFT:
        PRJ_setSlow( true );
        break;
    }
}

void handleKeyUpEvent( SDL_Event* event ) {
    switch ( event->key.keysym.scancode ) {
    case SDL_SCANCODE_LSHIFT:
        PRJ_setSlow( false );
        break;
    }
}

void handleKeys( bool *quit ) {
    const Uint8* keyStates = SDL_GetKeyboardState( NULL );
    if ( ( keyStates[SDL_SCANCODE_LALT] && keyStates[SDL_SCANCODE_RETURN] ) ||
           keyStates[SDL_SCANCODE_F11] ) {
        bool isFullscreen = ( SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN );
        SDL_Log( "Window set to %s mode.", ( isFullscreen ? "window" : "fullscreen" ) );
        SDL_SetWindowFullscreen( window, (SDL_bool)!isFullscreen );
    }
    if ( keyStates[SDL_SCANCODE_ESCAPE] ) { *quit = true; }
}

void handleControlKeys() {
    const Uint8* keyStates = SDL_GetKeyboardState( NULL );
    if ( keyStates[SDL_SCANCODE_W] ) { PRJ_moveForward(); }
    if ( keyStates[SDL_SCANCODE_A] ) { PRJ_moveLeft(); }
    if ( keyStates[SDL_SCANCODE_S] ) { PRJ_moveBack(); }
    if ( keyStates[SDL_SCANCODE_D] ) { PRJ_moveRight(); }
    if ( keyStates[SDL_SCANCODE_SPACE] ) { PRJ_moveUp(); }
    if ( keyStates[SDL_SCANCODE_LCTRL] ) { PRJ_moveDown(); }
}

void render( int *fps ) {
    //clear screen
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF ); //black
    SDL_RenderClear(renderer);

    //render objects
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF ); //white
    PRJ_update( *fps );
    PRJ_render( renderer, resolution );

    //Update the surface
    SDL_RenderPresent( renderer );
    SDL_UpdateWindowSurface( window );
}

void cleanup() {
    SDL_DestroyWindow( window );
    window = NULL;
    SDL_DestroyRenderer( renderer );
    renderer = NULL;

    SDL_Quit();
}
