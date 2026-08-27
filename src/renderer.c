#include "renderer.h"

#include <math.h>

#include <SDL3/SDL.h>

/*
 * Private rendering helpers
 */

static bool renderTextureInRectangle(
    SDL_Renderer *renderer,
    SDL_Texture *texture,
    const LayoutRectangle *rectangle)
{
    if (renderer == NULL ||
        texture == NULL ||
        rectangle == NULL) {
        return false;
    }

    const SDL_FRect destination = {
        .x = rectangle->x,
        .y = rectangle->y,
        .w = rectangle->width,
        .h = rectangle->height
    };

    return SDL_RenderTexture(
        renderer,
        texture,
        NULL,
        &destination
    );
}

static bool renderTextTexture(
    SDL_Renderer *renderer,
    const TextTexture *textTexture,
    float x,
    float y)
{
    if (renderer == NULL ||
        textTexture == NULL ||
        textTexture->texture == NULL) {
        return false;
    }

    const SDL_FRect destination = {
        .x = x,
        .y = y,
        .w = textTexture->width,
        .h = textTexture->height
    };

    return SDL_RenderTexture(
        renderer,
        textTexture->texture,
        NULL,
        &destination
    );
}

static bool renderVisualizer(
    const App *app,
    float timeSeconds)
{
    if (app == NULL || app->renderer == NULL) {
        return false;
    }

    const LayoutRectangle *area =
        &app->config.visualizer;

    const int barCount =
        app->config.visualizerBars;

    if (barCount <= 0 ||
        area->width <= 0.0f ||
        area->height <= 0.0f) {
        return false;
    }

    const float gap = 2.0f;

    const float totalGap =
        gap * (float)(barCount - 1);

    const float barWidth =
        (area->width - totalGap) /
        (float)barCount;

    if (barWidth <= 0.0f) {
        return false;
    }

    if (!SDL_SetRenderDrawColor(
            app->renderer,
            255,
            170,
            60,
            255)) {
        return false;
    }

    for (int barIndex = 0;
         barIndex < barCount;
         ++barIndex) {

        const float normalizedPosition =
            (float)barIndex /
            (float)barCount;

        /*
         * Temporary test animation.
         *
         * This will eventually be replaced by bar levels
         * supplied by the audio-analysis subsystem.
         */
        const float waveA =
            sinf(
                timeSeconds * 2.8f +
                normalizedPosition * 11.0f
            );

        const float waveB =
            sinf(
                timeSeconds * 1.4f -
                normalizedPosition * 19.0f
            );

        float normalizedHeight =
            0.5f +
            0.30f * waveA +
            0.20f * waveB;

        if (normalizedHeight < 0.05f) {
            normalizedHeight = 0.05f;
        }

        if (normalizedHeight > 1.0f) {
            normalizedHeight = 1.0f;
        }

        const float barHeight =
            area->height * normalizedHeight;

        const SDL_FRect bar = {
            .x =
                area->x +
                (float)barIndex *
                (barWidth + gap),

            .y =
                area->y +
                area->height -
                barHeight,

            .w = barWidth,
            .h = barHeight
        };

        if (!SDL_RenderFillRect(
                app->renderer,
                &bar)) {
            return false;
        }
    }

    return true;
}

static bool renderSafeAreaGuides(
    const App *app)
{
    if (app == NULL || app->renderer == NULL) {
        return false;
    }

    const float width =
        (float)app->config.logicalWidth -
        app->config.safeLeft -
        app->config.safeRight;

    const float height =
        (float)app->config.logicalHeight -
        app->config.safeTop -
        app->config.safeBottom;

    /*
     * Guides are optional, so an invalid guide area should
     * not stop the entire application from rendering.
     */
    if (width <= 0.0f || height <= 0.0f) {
        return true;
    }

    const SDL_FRect safeArea = {
        .x = app->config.safeLeft,
        .y = app->config.safeTop,
        .w = width,
        .h = height
    };

    if (!SDL_SetRenderDrawColor(
            app->renderer,
            110,
            110,
            110,
            255)) {
        return false;
    }

    return SDL_RenderRect(
        app->renderer,
        &safeArea
    );
}

/*
 * Public renderer interface
 */

bool rendererRenderFrame(
    const App *app)
{
    if (app == NULL || app->renderer == NULL) {
        return false;
    }

    /*
     * Clear the entire frame to the near-black
     * CRT background color.
     */
    if (!SDL_SetRenderDrawColor(
            app->renderer,
            5,
            4,
            3,
            255)) {
        return false;
    }

    if (!SDL_RenderClear(app->renderer)) {
        return false;
    }

    /*
     * Album artwork
     */
    if (app->assets.albumArt != NULL) {
        if (!renderTextureInRectangle(
                app->renderer,
                app->assets.albumArt,
                &app->config.albumArt)) {
            return false;
        }
    }

    /*
     * Track title
     */
    if (app->assets.title.texture != NULL) {
        if (!renderTextTexture(
                app->renderer,
                &app->assets.title,
                app->config.titleX,
                app->config.titleY)) {
            return false;
        }
    }

    /*
     * Artist name
     */
    if (app->assets.artist.texture != NULL) {
        if (!renderTextTexture(
                app->renderer,
                &app->assets.artist,
                app->config.artistX,
                app->config.artistY)) {
            return false;
        }
    }

    /*
     * Temporary animated visualizer
     */
    const float timeSeconds =
        (float)(
            (double)SDL_GetTicks() /
            1000.0
        );

    if (!renderVisualizer(
            app,
            timeSeconds)) {
        return false;
    }

    /*
     * Optional development guides
     */
    if (app->showGuides) {
        if (!renderSafeAreaGuides(app)) {
            return false;
        }
    }

    /*
     * Present the completed backbuffer.
     */
    return SDL_RenderPresent(app->renderer);
}