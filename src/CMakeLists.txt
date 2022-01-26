set(BL_STATIC 1)
find_path(BLEND_INC blend2d.h)
include_directories(${BLEND_INC})
find_library(BLEND_LIB blend2d)
message("${BLEND_LIB}")


project(blendplot CXX)

find_package(xtensor CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
#pybind11_add_module(pyblend2d src/pyblend2d.cpp src/plot.cpp)

add_executable(blendplot "canvas.h"
                         "artists.h"
                         "axis.h"
                         "common.h" 
                         "blendplot.cpp"
                         "turbo_cmap.h")

target_link_libraries(blendplot PRIVATE "${BLEND_LIB}" xtensor xtensor::optimize xtensor::use_xsimd 
                                PRIVATE SDL2::SDL2 SDL2::SDL2main
                                PRIVATE Catch2::Catch2)

file(INSTALL xkcd_colors.txt DESTINATION .)
