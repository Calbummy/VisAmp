cmake_minimum_required(VERSION 3.25)

project(CRT_Visualizer LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Project paths
#

set(EXTERNAL_LIBS_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/ExternalLibs"
)

set(SDL3_BASE
        "${EXTERNAL_LIBS_DIR}/SDL3-3.4.12"
)

set(SDL3_IMAGE_BASE
        "${EXTERNAL_LIBS_DIR}/SDL3_image-3.4.4"
)

set(SDL3_TTF_BASE
        "${EXTERNAL_LIBS_DIR}/SDL3_ttf-3.2.2"
)

#
# cJSON
#

add_library(cJSON STATIC
        "${EXTERNAL_LIBS_DIR}/cJSON-1.7.19/cJSON.c"
)

target_include_directories(cJSON
        PUBLIC
        "${EXTERNAL_LIBS_DIR}/cJSON-1.7.19"
)

#
# SDL package locations
#

set(SDL3_DIR
        "${SDL3_BASE}/cmake"
)

set(SDL3_image_DIR
        "${SDL3_IMAGE_BASE}/cmake"
)

set(SDL3_ttf_DIR
        "${SDL3_TTF_BASE}/cmake"
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

#
# Main application
#

add_executable(CRT_Visualizer
        src/main.c
        src/app.c
        src/assets.c
        src/config.c
        src/renderer.c

        src/app.h
        src/assets.h
        src/config.h
        src/renderer.h
)

target_include_directories(CRT_Visualizer
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(CRT_Visualizer
        PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        cJSON
)

if(MINGW)
    target_compile_options(CRT_Visualizer
            PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

#
# Copy linked runtime DLLs
#

if(WIN32)
    add_custom_command(
            TARGET CRT_Visualizer
            POST_BUILD

            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            $<TARGET_RUNTIME_DLLS:CRT_Visualizer>
            $<TARGET_FILE_DIR:CRT_Visualizer>

            COMMAND_EXPAND_LISTS

            COMMENT "Copying runtime DLLs"
            VERBATIM
    )
endif()

#
# Copy runtime configuration and assets
#

add_custom_command(
        TARGET CRT_Visualizer
        POST_BUILD

        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:CRT_Visualizer>/config"

        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:CRT_Visualizer>/assets"

        COMMENT "Copying configuration and assets"
        VERBATIM
)