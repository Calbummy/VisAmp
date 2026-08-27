#ifndef CRT_VISUALIZER_APP_H
#define CRT_VISUALIZER_APP_H

#include <stdbool.h>

#include <SDL3/SDL.h>

#include "assets.h"
#include "config.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    AppConfig config;
    Assets assets;

    bool running;
    bool showGuides;
    bool physicalSizeMode;
} App;

typedef enum {
    DISPLAY_MODE_TRACK,
    DISPLAY_MODE_AMBIENT,
    DISPLAY_MODE_SLEEP
} DisplayMode;

bool appInit(App *app);
void appRun(App *app);
void appShutdown(App *app);

#endif