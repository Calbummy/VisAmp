#include "app.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "renderer.h"

static void appHandleEvent(
    App *app,
    const SDL_Event *event)
{
    if (app == NULL || event == NULL) {
        return;
    }

    switch (event->type) {
        case SDL_EVENT_QUIT:
            app->running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key) {
                case SDLK_ESCAPE:
                    app->running = false;
                    break;

                case SDLK_G:
                    app->showGuides =
                        !app->showGuides;
                    break;

                default:
                    break;
            }

            break;

        default:
            break;
    }
}

bool appInit(App *app)
{
    if (app == NULL) {
        return false;
    }

    *app = (App){0};

    if (!SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_AUDIO)) {
        SDL_Log(
            "Could not initialize SDL: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!TTF_Init()) {
        SDL_Log(
            "Could not initialize SDL_ttf: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!configLoad(
            &app->config,
            CONFIG_PATH)) {
        SDL_Log(
            "Using default layout configuration"
        );
    }

    if (!SDL_CreateWindowAndRenderer(
            "CRT Music Visualizer",
            app->config.logicalWidth * 3,
            app->config.logicalHeight * 3,
            0,
            &app->window,
            &app->renderer)) {

        SDL_Log(
            "Could not create window and renderer: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!SDL_SetRenderVSync(app->renderer, 1)) {
        SDL_Log(
            "Could not enable VSync: %s",
            SDL_GetError()
        );
    }

    if (!SDL_SetRenderLogicalPresentation(
            app->renderer,
            app->config.logicalWidth,
            app->config.logicalHeight,
            SDL_LOGICAL_PRESENTATION_LETTERBOX)) {

        SDL_Log(
            "Could not set logical presentation: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!assetsLoad(
            &app->assets,
            app->renderer,
            &app->config,
            "Test Track",
            "Test Artist")) {

        SDL_Log("Could not load required assets");
        return false;
    }

    app->running = true;
    app->showGuides = false;
    app->physicalSizeMode = false;

    return true;
}

void appRun(App *app)
{
    if (app == NULL) {
        return;
    }

    while (app->running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            appHandleEvent(app, &event);
        }

        if (!rendererRenderFrame(app)) {
            SDL_Log(
                "Rendering failed: %s",
                SDL_GetError()
            );

            app->running = false;
        }
    }
}

void appShutdown(App *app)
{
    if (app == NULL) {
        return;
    }

    assetsDestroy(&app->assets);

    if (app->renderer != NULL) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = NULL;
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
    }

    TTF_Quit();
    SDL_Quit();

    *app = (App){0};
}