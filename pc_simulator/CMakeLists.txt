cmake_minimum_required(VERSION 3.16)
project(lvgl_pc_simulator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2 dependency on Linux using PkgConfig
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)

# Include directories
include_directories(
    .
    ../Middlewares/Third_Party/lvgl
    ../Middlewares/Third_Party
    ../CM4/Core/Src/eez_ui
    ../CM4/Core/Inc
    ../Common
    ${SDL2_INCLUDE_DIRS}
)

# Core LVGL sources (from shared Middlewares folder)
file(GLOB_RECURSE LVGL_SOURCES
    "../Middlewares/Third_Party/lvgl/src/*.c"
)

# EEZ-ported UI sources (output of port_eez_ui.py)
file(GLOB APP_SOURCES
    "../CM4/Core/Src/eez_ui/*.c"
)

# PC Simulator porting and entry sources
set(SIMULATOR_SOURCES
    main.c
    sdl_port.c
    pc_simulator_hw.c
)

# Build Target Executable
add_executable(lvgl_simulator
    ${LVGL_SOURCES}
    ${APP_SOURCES}
    ${SIMULATOR_SOURCES}
)

# Redirect SHARED_BUFFER macro to the simulator's mock struct instead of 0x30040000
# Also define PC_SIMULATOR so any conditional code can detect the host build
target_compile_definitions(lvgl_simulator PRIVATE
    PC_SIMULATOR=1
    "SHARED_BUFFER=((volatile SharedBuffer_t*)&sim_shared_buffer)"
)

# Link SDL2 libraries
target_link_libraries(lvgl_simulator ${SDL2_LIBRARIES})

# Enable basic debugging flags; suppress common LVGL warnings
target_compile_options(lvgl_simulator PRIVATE -Wall -Wextra -g -O2
    -Wno-unused-parameter -Wno-sign-compare)
