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

set(CRT0_COMPAT_FLAGS
    -Wno-error=implicit-function-declaration
    -Wno-error=int-conversion
    -Wno-error=implicit-int
    -Wno-error=declaration-missing-parameter-type
    -Wno-error=builtin-declaration-mismatch
)

add_library(crt0_main OBJECT crt0_main.c)
target_compile_options(crt0_main PRIVATE ${CRT0_COMPAT_FLAGS})

add_library(crt0_module OBJECT crt0_module.c)
target_compile_options(crt0_module PRIVATE ${CRT0_COMPAT_FLAGS})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/crt0_main.dir/crt0_main.c.obj
    DESTINATION lib
    RENAME crt0_main.o
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/crt0_module.dir/crt0_module.c.obj
    DESTINATION lib
    RENAME crt0_module.o
)
