#ifndef CRT_VISUALIZER_CONFIG_H
#define CRT_VISUALIZER_CONFIG_H

#include <stdbool.h>
#define CONFIG_PATH "config/layout.json"
#define MAX_VISUALIZER_BARS 256
#define CONFIG_PATH_LENGTH 260

typedef struct {
    float x;
    float y;
    float width;
    float height;
} LayoutRectangle;

typedef struct {
    int logicalWidth;
    int logicalHeight;

    float physicalWidthInches;
    float physicalHeightInches;
    float previewPixelsPerInch;

    char fontPath[CONFIG_PATH_LENGTH];
    float fontSize;

    char albumArtPath[CONFIG_PATH_LENGTH];

    LayoutRectangle albumArt;

    float titleX;
    float titleY;

    float artistX;
    float artistY;

    LayoutRectangle visualizer;
    int visualizerBars;

    float safeLeft;
    float safeRight;
    float safeTop;
    float safeBottom;
} AppConfig;

void configSetDefaults(AppConfig *config);

bool configLoad(
    AppConfig *config,
    const char *path);

#endif