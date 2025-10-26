cmake_minimum_required(VERSION 4.1.2)
project(rol-font-renderer C)

add_library(rol-font-renderer
  src/loaders/bdf.c
  src/font_loader.c
  src/errors.c
  src/display/string.c
  src/display/texture.c
)

target_include_directories(rol-font-renderer PUBLIC include)
add_subdirectory(examples)
