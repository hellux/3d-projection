#include <SDL2/SDL.h>

void initWindow(void);
void initProjector(void);
void pollEvents(SDL_Event* event, bool *quit);
void handleWindowEvent(SDL_Event* event);
void handleKeyDownEvent(SDL_Event* event);
void handleKeyUpEvent(SDL_Event* event);
void handleKeys(bool *quit);
void handleControlKeys(void);
void render(int *fps);
void cleanup(void);
void test(void);

#include "projector.hpp"
