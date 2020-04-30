cmake_minimum_required(VERSION 3.4.0)

project(blendplot C CXX)
#add_subdirectory(src/pybind11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20) 



message("$ENV{VCPKG_ROOT} Neeein")

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()


set(BL_STATIC 1)

find_path(BLEND_INC blend2d.h)
include_directories(${BLEND_INC})
find_library(BLEND_LIB blend2d)
message("${BLEND_LIB}")
#link_libraries(${BLEND_LIB})



find_package(xtensor CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

#pybind11_add_module(pyblend2d src/pyblend2d.cpp src/plot.cpp)
add_executable(blendplot "src/canvas.h" "src/artists.h"  "src/axis.h" "src/common.h" "src/blendplot.cpp" "src/turbo_cmap.h")


target_link_libraries(blendplot PRIVATE "${BLEND_LIB}" xtensor xtensor::optimize xtensor::use_xsimd SDL2::SDL2 SDL2::SDL2main
                                PRIVATE ${xtensor_INCLUDE_DIRS}
                                PRIVATE Catch2::Catch2)                    
target_include_directories(blendplot PRIVATE ${BLEND_INC} blend2d SDL2::SDL2 )

add_executable(blendtest "src/test_blend2d.cpp" "src/turbo_cmap.h")
target_link_libraries(blendtest PRIVATE "${BLEND_LIB}" )                    
target_include_directories(blendtest PRIVATE ${BLEND_INC} blend2d)
file(INSTALL src/xkcd_colors.txt DESTINATION .)



#add_executable (blendplot-test "src/blendplot.cpp")
#target_link_libraries(blendplot-test PRIVATE SDL2::SDL2 SDL2::SDL2main)
#target_link_libraries (blendplot-test PRIVATE blendplot )
#target_link_directories (blendplot-test blendplot)
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/hello")
