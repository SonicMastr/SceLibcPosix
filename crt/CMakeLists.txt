cmake_minimum_required(VERSION 3.7)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(crt0)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

include_directories(../include)

add_library(crt0_main OBJECT crt0_main.c)
add_library(crt0_module OBJECT crt0_module.c)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/crt0_main.dir/crt0_main.c.obj
    DESTINATION lib
    RENAME crt0_main.o
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/crt0_module.dir/crt0_module.c.obj
    DESTINATION lib
    RENAME crt0_module.o
)
