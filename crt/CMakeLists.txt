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

add_library(crt0_main OBJECT crt0_main.c)
add_library(crt0_module OBJECT crt0_module.c)
