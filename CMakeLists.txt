cmake_minimum_required(VERSION 3.4.0)
project(blendplot)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

add_subdirectory(src)


#add_executable (blendplot-test "src/blendplot.cpp")
#target_link_libraries(blendplot-test PRIVATE SDL2::SDL2 SDL2::SDL2main)
#target_link_libraries (blendplot-test PRIVATE blendplot )
#target_link_directories (blendplot-test blendplot)
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/hello")
