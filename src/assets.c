#include "assets.h"

#include <stddef.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

static void textTextureDestroy(TextTexture *textTexture)
{
    if (textTexture == NULL) {
        return;
    }

    if (textTexture->texture != NULL) {
        SDL_DestroyTexture(textTexture->texture);
    }

    *textTexture = (TextTexture){0};
}

static SDL_Texture *imageTextureLoad(
    SDL_Renderer *renderer,
    const char *path)
{
    if (renderer == NULL ||
        path == NULL ||
        path[0] == '\0') {
        return NULL;
    }

    SDL_Texture *texture =
        IMG_LoadTexture(renderer, path);

    if (texture == NULL) {
        SDL_Log(
            "Could not load image '%s': %s",
            path,
            SDL_GetError()
        );

        return NULL;
    }

    return texture;
}

static bool textTextureBuild(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    SDL_Color color,
    TextTexture *output)
{
    if (renderer == NULL ||
        font == NULL ||
        text == NULL ||
        output == NULL) {
        return false;
    }

    *output = (TextTexture){0};

    SDL_Surface *surface =
        TTF_RenderText_Blended(
            font,
            text,
            0,
            color
        );

    if (surface == NULL) {
        SDL_Log(
            "Could not render '%s': %s",
            text,
            SDL_GetError()
        );

        return false;
    }

    output->width = (float)surface->w;
    output->height = (float)surface->h;

    output->texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

    SDL_DestroySurface(surface);

    if (output->texture == NULL) {
        SDL_Log(
            "Could not create texture for '%s': %s",
            text,
            SDL_GetError()
        );

        output->width = 0.0f;
        output->height = 0.0f;

        return false;
    }

    return true;
}

void assetsDestroy(Assets *assets)
{
    if (assets == NULL) {
        return;
    }

    textTextureDestroy(&assets->title);
    textTextureDestroy(&assets->artist);

    if (assets->albumArt != NULL) {
        SDL_DestroyTexture(assets->albumArt);
        assets->albumArt = NULL;
    }

    if (assets->font != NULL) {
        TTF_CloseFont(assets->font);
        assets->font = NULL;
    }
}

bool assetsLoad(
    Assets *assets,
    SDL_Renderer *renderer,
    const AppConfig *config,
    const char *titleText,
    const char *artistText)
{
    if (assets == NULL ||
        renderer == NULL ||
        config == NULL ||
        titleText == NULL ||
        artistText == NULL) {
        return false;
    }

    SDL_Texture *newAlbumArt = NULL;
    TTF_Font *newFont = NULL;

    TextTexture newTitle = {0};
    TextTexture newArtist = {0};

    const SDL_Color textColor = {
        .r = 235,
        .g = 235,
        .b = 220,
        .a = 255
    };

    /*
     * Album art can be optional during development.
     */
    newAlbumArt =
        imageTextureLoad(
            renderer,
            config->albumArtPath
        );

    newFont =
        TTF_OpenFont(
            config->fontPath,
            config->fontSize
        );

    if (newFont == NULL) {
        SDL_Log(
            "Could not open font '%s': %s",
            config->fontPath,
            SDL_GetError()
        );

        goto loadFailure;
    }

    if (!textTextureBuild(
            renderer,
            newFont,
            titleText,
            textColor,
            &newTitle)) {
        goto loadFailure;
    }

    if (!textTextureBuild(
            renderer,
            newFont,
            artistText,
            textColor,
            &newArtist)) {
        goto loadFailure;
    }

    assetsDestroy(assets);

    assets->albumArt = newAlbumArt;
    assets->font = newFont;
    assets->title = newTitle;
    assets->artist = newArtist;

    return true;

loadFailure:
    textTextureDestroy(&newTitle);
    textTextureDestroy(&newArtist);

    if (newAlbumArt != NULL) {
        SDL_DestroyTexture(newAlbumArt);
    }

    if (newFont != NULL) {
        TTF_CloseFont(newFont);
    }

    return false;
}

bool assetsUpdateText(
    Assets *assets,
    SDL_Renderer *renderer,
    const char *titleText,
    const char *artistText)
{
    if (assets == NULL ||
        renderer == NULL ||
        assets->font == NULL ||
        titleText == NULL ||
        artistText == NULL) {
        return false;
    }

    const SDL_Color textColor = {
        .r = 235,
        .g = 235,
        .b = 220,
        .a = 255
    };

    TextTexture newTitle = {0};
    TextTexture newArtist = {0};

    if (!textTextureBuild(
            renderer,
            assets->font,
            titleText,
            textColor,
            &newTitle)) {
        return false;
    }

    if (!textTextureBuild(
            renderer,
            assets->font,
            artistText,
            textColor,
            &newArtist)) {
        textTextureDestroy(&newTitle);
        return false;
    }

    textTextureDestroy(&assets->title);
    textTextureDestroy(&assets->artist);

    assets->title = newTitle;
    assets->artist = newArtist;

    return true;
}