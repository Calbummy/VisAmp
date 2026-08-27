#include "config.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include "cJSON.h"

void configSetDefaults(AppConfig *config)
{
    if (config == NULL) {
        return;
    }

    *config = (AppConfig) {
        .logicalWidth = 320,
        .logicalHeight = 240,

        .physicalWidthInches = 4.0f,
        .physicalHeightInches = 3.0f,
        .previewPixelsPerInch = 96.0f,

        /*
         * Keep paths lowercase because Linux paths are case-sensitive.
         * Replace these placeholder filenames with real assets later.
         */
        .fontPath = "assets/fonts/JetBrainsMono-Regular.ttf",
        .fontSize = 15.0f,

        .albumArtPath = "assets/images/Icon_Bird_512x512.png",

        .albumArt = {
            .x = 10.0f,
            .y = 10.0f,
            .width = 104.0f,
            .height = 104.0f
        },

        .titleX = 126.0f,
        .titleY = 14.0f,

        .artistX = 126.0f,
        .artistY = 39.0f,

        .visualizer = {
            .x = 10.0f,
            .y = 150.0f,
            .width = 300.0f,
            .height = 72.0f
        },

        .visualizerBars = 24,

        .safeLeft = 8.0f,
        .safeRight = 8.0f,
        .safeTop = 8.0f,
        .safeBottom = 8.0f
    };
}

static char *readEntireFile(const char *path)
{
    if (path == NULL) {
        return NULL;
    }

    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    const long length = ftell(file);

    if (length < 0) {
        fclose(file);
        return NULL;
    }

    rewind(file);

    char *buffer = malloc((size_t)length + 1U);

    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    const size_t bytesRead = fread(
        buffer,
        1U,
        (size_t)length,
        file
    );

    fclose(file);

    if (bytesRead != (size_t)length) {
        free(buffer);
        return NULL;
    }


    buffer[(size_t)length] = '\0';

    return buffer;
}

static void jsonReadInt(
    const cJSON *object,
    const char *name,
    int *destination)
{
    if (object == NULL ||
        name == NULL ||
        destination == NULL) {
        return;
        }

    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, name);

    if (cJSON_IsNumber(item)) {
        *destination = item->valueint;
    }
}

static void jsonReadFloat(
    const cJSON *object,
    const char *name,
    float *destination)
{
    if (object == NULL ||
        name == NULL ||
        destination == NULL) {
        return;
        }

    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, name);

    if (cJSON_IsNumber(item)) {
        *destination = (float)item->valuedouble;
    }
}


static void jsonReadString(
    const cJSON *object,
    const char *name,
    char *destination,
    size_t destinationSize)
{
    if (object == NULL ||
        name == NULL ||
        destination == NULL ||
        destinationSize == 0U) {
        return;
        }

    const cJSON *item =
        cJSON_GetObjectItemCaseSensitive(object, name);

    if (!cJSON_IsString(item) ||
        item->valuestring == NULL) {
        return;
        }

    SDL_strlcpy(
        destination,
        item->valuestring,
        destinationSize
    );
}


static void jsonReadRect(
    const cJSON *parent,
    const char *name,
    LayoutRectangle *rect)
{
    if (parent == NULL ||
        name == NULL ||
        rect == NULL) {
        return;
        }

    const cJSON *object =
        cJSON_GetObjectItemCaseSensitive(parent, name);

    if (!cJSON_IsObject(object)) {
        return;
    }

    jsonReadFloat(object, "x", &rect->x);
    jsonReadFloat(object, "y", &rect->y);
    jsonReadFloat(object, "width", &rect->width);
    jsonReadFloat(object, "height", &rect->height);
}

static bool configValidate(AppConfig *config)
{
    if (config == NULL) {
        return false;
    }


    if (config->logicalWidth <= 0 ||
        config->logicalHeight <= 0) {
        SDL_Log(
            "Invalid logical resolution; restoring configuration defaults"
        );

        configSetDefaults(config);
        return false;
        }

    if (config->physicalWidthInches <= 0.0f) {
        SDL_Log(
            "Invalid physicalWidthInches; restoring default"
        );

        config->physicalWidthInches = 4.0f;
    }

    if (config->physicalHeightInches <= 0.0f) {
        SDL_Log(
            "Invalid physicalHeightInches; restoring default"
        );

        config->physicalHeightInches = 3.0f;
    }

    if (config->previewPixelsPerInch <= 0.0f) {
        SDL_Log(
            "Invalid previewPixelsPerInch; restoring default"
        );

        config->previewPixelsPerInch = 96.0f;
    }

    if (config->fontSize <= 0.0f) {
        SDL_Log(
            "Invalid fontSize; restoring default"
        );

        config->fontSize = 15.0f;
    }


    if (config->albumArt.width < 0.0f) {
        config->albumArt.width = 0.0f;
    }

    if (config->albumArt.height < 0.0f) {
        config->albumArt.height = 0.0f;
    }

    if (config->visualizer.width < 0.0f) {
        config->visualizer.width = 0.0f;
    }

    if (config->visualizer.height < 0.0f) {
        config->visualizer.height = 0.0f;
    }

    if (config->visualizerBars < 1) {
        config->visualizerBars = 1;
    }

    if (config->visualizerBars > MAX_VISUALIZER_BARS) {
        config->visualizerBars = MAX_VISUALIZER_BARS;
    }

    if (config->safeLeft < 0.0f) {
        config->safeLeft = 0.0f;
    }

    if (config->safeRight < 0.0f) {
        config->safeRight = 0.0f;
    }

    if (config->safeTop < 0.0f) {
        config->safeTop = 0.0f;
    }

    if (config->safeBottom < 0.0f) {
        config->safeBottom = 0.0f;
    }

    return true;
}


bool configLoad(
    AppConfig *config,
    const char *path)
{
    if (config == NULL || path == NULL) {
        return false;
    }

    configSetDefaults(config);

    char *text = readEntireFile(path);

    if (text == NULL) {
        SDL_Log(
            "Could not read %s; using defaults",
            path
        );

        return false;
    }

    cJSON *root = cJSON_Parse(text);

    free(text);

    if (root == NULL) {
        SDL_Log(
            "Could not parse %s; using defaults",
            path
        );

        return false;
    }


    if (!cJSON_IsObject(root)) {
        SDL_Log(
            "Root of %s is not a JSON object; using defaults",
            path
        );

        cJSON_Delete(root);
        return false;
    }

    jsonReadInt(
        root,
        "logicalWidth",
        &config->logicalWidth
    );

    jsonReadInt(
        root,
        "logicalHeight",
        &config->logicalHeight
    );

    jsonReadFloat(
        root,
        "physicalWidthInches",
        &config->physicalWidthInches
    );

    jsonReadFloat(
        root,
        "physicalHeightInches",
        &config->physicalHeightInches
    );

    jsonReadFloat(
        root,
        "previewPixelsPerInch",
        &config->previewPixelsPerInch
    );

    jsonReadString(
        root,
        "fontPath",
        config->fontPath,
        sizeof(config->fontPath)
    );

    jsonReadFloat(
        root,
        "fontSize",
        &config->fontSize
    );

    jsonReadString(
        root,
        "albumArtPath",
        config->albumArtPath,
        sizeof(config->albumArtPath)
    );

    jsonReadRect(
        root,
        "albumArt",
        &config->albumArt
    );

    jsonReadRect(
        root,
        "visualizer",
        &config->visualizer
    );

    const cJSON *title =
        cJSON_GetObjectItemCaseSensitive(root, "title");

    if (cJSON_IsObject(title)) {
        jsonReadFloat(
            title,
            "x",
            &config->titleX
        );

        jsonReadFloat(
            title,
            "y",
            &config->titleY
        );
    }

    const cJSON *artist =
        cJSON_GetObjectItemCaseSensitive(root, "artist");

    if (cJSON_IsObject(artist)) {
        jsonReadFloat(
            artist,
            "x",
            &config->artistX
        );

        jsonReadFloat(
            artist,
            "y",
            &config->artistY
        );
    }

    const cJSON *visualizer =
        cJSON_GetObjectItemCaseSensitive(root, "visualizer");

    if (cJSON_IsObject(visualizer)) {
        jsonReadInt(
            visualizer,
            "bars",
            &config->visualizerBars
        );
    }

    const cJSON *safeArea =
        cJSON_GetObjectItemCaseSensitive(root, "safeArea");

    if (cJSON_IsObject(safeArea)) {
        jsonReadFloat(
            safeArea,
            "left",
            &config->safeLeft
        );

        jsonReadFloat(
            safeArea,
            "right",
            &config->safeRight
        );

        jsonReadFloat(
            safeArea,
            "top",
            &config->safeTop
        );

        jsonReadFloat(
            safeArea,
            "bottom",
            &config->safeBottom
        );
    }

    /*
     * Release the memory allocated by cJSON_Parse.
     */
    cJSON_Delete(root);

    return configValidate(config);
}