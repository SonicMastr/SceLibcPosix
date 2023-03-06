cmake_minimum_required(VERSION 3.7)

option(BUILD_TESTS "Build test programs" OFF)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project(SceLibcPosix)


add_subdirectory(SceLibcExt)
add_subdirectory(crt)
add_subdirectory(src)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
