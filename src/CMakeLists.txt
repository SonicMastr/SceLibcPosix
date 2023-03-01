cmake_minimum_required(VERSION 3.7)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(libSceLibcExt)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

file(GLOB_RECURSE sources *.c)

add_executable(SceLibcExt $<TARGET_OBJECTS:crt0_module>
    ${sources}
)

target_compile_options(SceLibcExt PRIVATE -fno-lto -fpermissive)

target_include_directories(SceLibcExt PRIVATE include internal)

target_link_libraries(SceLibcExt PRIVATE
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    SceRtc_stub
    SceNet_stub_weak
    SceProcessmgr_stub
    SceKernelThreadMgr_stub
    SceFios2_stub_weak
    SceLibc_stub_weak
    SceLibKernel_stub
    gcc
)

vita_create_self(SceLibcExt.suprx SceLibcExt PRIVATE
    CONFIG exports.yml
    UNSAFE
)

vita_create_stubs(stubs SceLibcExt ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stubs/
    DESTINATION lib
    FILES_MATCHING PATTERN "*.a"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SceLibcExt.suprx
    DESTINATION "$ENV{VITASDK}/arm-vita-eabi/modules"
)
