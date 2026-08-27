#include <stdlib.h>

#include <SDL3/SDL_main.h>

#include "app.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    App app = {0};

    if (!appInit(&app)) {
        appShutdown(&app);
        return EXIT_FAILURE;
    }

    appRun(&app);
    appShutdown(&app);

    return EXIT_SUCCESS;
}