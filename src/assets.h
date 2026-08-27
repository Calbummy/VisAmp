#ifndef CRT_VISUALIZER_ASSETS_H
#define CRT_VISUALIZER_ASSETS_H

#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "config.h"

typedef struct {
    SDL_Texture *texture;
    float width;
    float height;
} TextTexture;

typedef struct {
    SDL_Texture *albumArt;
    TTF_Font *font;

    TextTexture title;
    TextTexture artist;
} Assets;

bool assetsLoad(
    Assets *assets,
    SDL_Renderer *renderer,
    const AppConfig *config,
    const char *titleText,
    const char *artistText
);

bool assetsUpdateText(
    Assets *assets,
    SDL_Renderer *renderer,
    const char *titleText,
    const char *artistText
);

void assetsDestroy(Assets *assets);

#endif